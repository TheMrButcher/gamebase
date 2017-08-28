/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/TextBox.h>
#include <gamebase/impl/textfilter/AutoLengthTextFilter.h>
#include <gamebase/impl/text/Conversion.h>
#include <gamebase/impl/text/Clipboard.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <locale>

namespace gamebase { namespace impl {

namespace {
bool isCharStartLess(const CharPosition& charPos, float x)
{
    return charPos.position.bottomLeft.x < x;
}
}

TextBox::TextBox(
    const std::shared_ptr<TextBoxSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_offsetX(0)
    , m_firstVisibleSymbol(0)
    , m_inited(false)
{}

void TextBox::setText(const std::string& text)
{
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_offsetX = 0;
    m_firstVisibleSymbol = 0;
    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    m_skin->setOffsetX(0);
    try {
        m_skin->setText(text);
    } catch (std::exception& ex) {
        std::cout << "Error while trying to set text \"" << text << "\" to TextEdit"
            ". Reason: " << ex.what() << std::endl;
        m_skin->setText(m_text.toString());
        return;
    }
    m_text.set(text);
    if (m_inited)
        m_skin->loadResources();
}

void TextBox::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Pressed)
        state = SelectionState::Selected;
    if (m_selectionState == SelectionState::Selected && state == SelectionState::MouseOn)
        return;
    bool finishedWork = false;
    if (m_selectionState == SelectionState::Selected && state != m_selectionState) {
        m_skin->setSelection(0, 0);
        if (state == SelectionState::None)
            finishedWork = true;
    }
    m_selectionState = state;
    m_skin->setSelectionState(state);

    if (finishedWork && m_callback)
        m_callback();
}

void TextBox::processInput(const InputRegister& input)
{
    const auto& keys = input.keys.signals();
    for (auto it = keys.begin(); it != keys.end(); ++it)
        processKey(*it);

    const auto& specialKeys = input.specialKeys.signals();
    for (auto it = specialKeys.begin(); it != specialKeys.end(); ++it)
        processSpecialKey(*it);

    processMouse(input);
}

void TextBox::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
    builder->registerProperty("text", &m_text.toString(),
		[this](const std::string& value) { setText(value); });
}

void TextBox::serialize(Serializer& s) const
{
    s << "text" << m_text.toString() << "position" << m_offset << "skin" << m_skin;
    if (dynamic_cast<const ISerializable*>(m_textFilter.get()))
        s << "filter" << m_textFilter;
    else
        s << "filter" << std::shared_ptr<ITextFilter>();
}

std::unique_ptr<IObject> deserializeTextBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<TextBoxSkin>, skin);
    DESERIALIZE(std::shared_ptr<ITextFilter>, filter);
    DESERIALIZE(std::string, text);
    std::unique_ptr<TextBox> result(new TextBox(skin, position));
    result->setText(text);
    result->setTextFilter(filter);
    return std::move(result);
}

REGISTER_CLASS(TextBox);

void TextBox::processKey(char key)
{
    static std::locale loc("");
    bool anyChange = false;
    auto selectionLeft = std::min(m_selectionStart, m_selectionEnd);
    auto selectionRight = std::max(m_selectionStart, m_selectionEnd);

	if (key == '\t')
		key = ' ';

    if (std::isprint(key, loc)) {
        auto newText = m_text;
        newText.erase(selectionLeft, selectionRight);
        auto utf8Code = convertToUtf8(std::string(1, key));
        newText.insert(selectionLeft, utf8Code);
        m_text = filterText(newText);

        setCursor(selectionLeft + 1);
        anyChange = true;
    }

    if (key == 1) {
        m_selectionStart = 0;
        m_selectionEnd = m_text.size();
        updateSkin();
        return;
    }
    
    if (key == 3 || key == 24) {
        if (selectionLeft != selectionRight)
            toClipboardUtf8(m_text.substr(selectionLeft, selectionRight - selectionLeft).toString());
    }

    if (key == 8 || key == 127 || key == 24) {
        if (selectionLeft == selectionRight) {
            if (key == 8) {
                if (selectionRight == 0)
                    return;
                else
                    selectionLeft = selectionRight - 1;
            }
            if (key == 127) {
                if (selectionLeft == m_text.size())
                    return;
                else
                    selectionRight = selectionLeft + 1;
            }
        }
        
        if (selectionLeft != selectionRight) {
            auto newText = m_text;
            newText.erase(selectionLeft, selectionRight);
            m_text = filterText(newText);

            setCursor(selectionLeft);
            anyChange = true;
        }
    }

    if (key == 22) {
        auto clipboardText = fromClipboardUtf8();
        if (!clipboardText.empty()) {
            auto newText = m_text;
            newText.erase(selectionLeft, selectionRight);
            newText.insert(selectionLeft, clipboardText);
            m_text = filterText(newText);
            setCursor(selectionLeft + Utf8Text(clipboardText).size());
            anyChange = true;
        }
    }

    if (key == 10 || key == 13) {
        setSelectionState(SelectionState::None);
        return;
    }

    if (anyChange) {
        m_skin->setText(m_text.toString());
        updateSkin();
    }
}
    
void TextBox::processSpecialKey(SpecialKey::Enum key)
{
    switch (key) {
        case SpecialKey::Left: moveCursor(-1); break;
        case SpecialKey::Right: moveCursor(+1); break;
        case SpecialKey::Home: setCursor(0); break;
        case SpecialKey::End: setCursor(m_text.size()); break;
        default: return;
    }

    updateSkin();
}

void TextBox::processMouse(const InputRegister& input)
{
    if (input.mouseButtons.isJustPressed(MouseButton::Left)
        || input.mouseButtons.isPressed(MouseButton::Left)) {
        float x = (fullTransform().inversed() * input.mousePosition()).x;
        if (input.mouseButtons.isJustPressed(MouseButton::Left)) {
            setCursor(calcCharIndex(x));
            timer.start();
        } else {
            if (!timer.isPeriod(m_skin->shiftPeriod()))
                return;
            size_t newEnd = calcCharIndex(x);
            if (newEnd == m_selectionEnd)
                return;
            m_selectionEnd = newEnd;
        }

        updateSkin();
    }
}

void TextBox::setCursor(size_t pos)
{
    pos = std::min(pos, m_text.size());
    m_selectionStart = pos;
    m_selectionEnd = pos;
}

void TextBox::moveCursor(int shift)
{
    size_t pos = shift < 0
        ? std::min(m_selectionStart, m_selectionEnd)
        : std::max(m_selectionStart, m_selectionEnd);
    if (shift < 0 && pos < static_cast<size_t>(-shift))
        pos = 0;
    else
        pos += shift;

    m_selectionStart = std::min(pos, m_text.size());
    m_selectionEnd = m_selectionStart;
}

size_t TextBox::calcCharIndex(float x)
{
    const auto& textGeom = m_skin->textGeometry();
    x -= m_offsetX;
    if (x <= textGeom.front().position.bottomLeft.x)
        return 0;
    if (x >= textGeom.back().position.bottomLeft.x)
        return m_text.size();
    auto it = std::lower_bound(textGeom.begin(), textGeom.end(),
        x, isCharStartLess);
    size_t index = static_cast<size_t>(it - textGeom.begin());
    if (index == 0)
        return 0;
    else
        return index - 1;
}

Utf8Text TextBox::filterText(const Utf8Text& newText) const
{
    AutoLengthTextFilter prefilter(m_skin.get());
    auto filtered = prefilter.filter(m_text, newText);
    if (m_textFilter)
        filtered = m_textFilter->filter(m_text, newText);
    return std::move(filtered);
}

void TextBox::updateSkin()
{
    const auto& textGeom = m_skin->textGeometry();
    if (textGeom.size() > 1) {
        float startX = textGeom[0].position.bottomLeft.x;
        auto box = m_skin->textAreaBox();

        size_t shouldBeVisibleIndex = m_selectionStart == m_selectionEnd
            ? (m_selectionStart == m_text.size() ? m_text.size() - 1 : m_selectionStart)
            : m_selectionStart < m_selectionEnd
                ? m_selectionEnd - 1
                : m_selectionEnd;

        const float leftX = textGeom[shouldBeVisibleIndex].position.bottomLeft.x + m_offsetX;
        const float rightX = textGeom[shouldBeVisibleIndex + 1].position.bottomLeft.x + m_offsetX;
        if (leftX < box.bottomLeft.x) {
            m_firstVisibleSymbol = shouldBeVisibleIndex;
        } else if (rightX > box.topRight.x) {
            float w = box.width();
            size_t i = shouldBeVisibleIndex;
            float curLeftX = leftX;
            float curRightX = rightX;
            bool notEnoughSpace = false;
            for (;;) {
                w += curLeftX - curRightX;
                if (w < 0) {
                    notEnoughSpace = true;
                    break;
                }
                if (i == 0)
                    break;
                --i;
                curRightX = curLeftX;
                curLeftX = textGeom[i].position.bottomLeft.x + m_offsetX;
            }
            m_firstVisibleSymbol = i;
            if (notEnoughSpace && m_firstVisibleSymbol < shouldBeVisibleIndex)
                m_firstVisibleSymbol++;
        }
        m_offsetX = startX - textGeom[m_firstVisibleSymbol].position.bottomLeft.x;
        m_skin->setOffsetX(m_offsetX);
    } else {
        m_firstVisibleSymbol = 0;
        m_offsetX = 0;
        m_skin->setOffsetX(0);
    }
    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    m_skin->loadResources();
}

} }
