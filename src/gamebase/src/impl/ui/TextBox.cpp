/**
 * Copyright (c) 2017 Slavnejshev Filipp
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
#include <SFML/System/String.hpp>
#include <locale>

namespace gamebase { namespace impl {

static std::locale LOCALE("");

namespace {
bool isCharStartLess(const CharPosition& charPos, float x)
{
    return charPos.position.bottomLeft.x < x;
}

uint32_t filter(uint32_t ch)
{
    sf::String str(ch);
    auto localStr = str.toAnsiString(LOCALE);
    if (localStr.size() == 1) {
        std::cerr << "New character: " << ch << ", is printable: " << std::boolalpha << std::isprint(localStr[0], LOCALE) << std::endl;
        if (!std::isprint(localStr[0], LOCALE))
            return 0;
    } else {
        std::cerr << "New character: " << ch << ", multichar, length: " << localStr.length() << std::endl;
        bool isAnyPrintable = false;
        int i = 0;
        for (auto localChar : localStr) {
            std::cerr << "char #" << i << ": " << static_cast<int>(localChar) << ", is printable: " << std::boolalpha << std::isprint(localChar, LOCALE) << std::endl;
            isAnyPrintable = isAnyPrintable || std::isprint(localChar, LOCALE);
        }
        if (!isAnyPrintable)
            return 0;
    }
    
    if (ch == '\n' || ch == '\r')
        return 0;
    if (ch == '\t')
        return ' ';
    return ch;
}

template <typename Collection>
std::string toString(const Collection& collection)
{
    std::string result;
    result.reserve(collection.size());
    for (const auto& ch : collection)
        result.push_back(static_cast<char>(ch));
    return result;
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
        m_callbackHandle = Handle(m_callback);
}

void TextBox::processInput(const InputRegister& input)
{
    if (m_selectionState == SelectionState::Selected) {
        processKeyboard(input);
        processMouse(input);
    }
}

void TextBox::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject(m_skin.get());
    builder->registerPropertyWithSetter("text", &m_text.toString(),
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

void TextBox::processKeyboard(const InputRegister& input)
{
    bool anyChange = !input.keys.signals().empty();
    bool isControlPressed =
        input.keys.isPressed(InputKey::CtrlLeft)
        || input.keys.isPressed(InputKey::CtrlRight);

    for (auto key : input.keys.signals()) {
        auto selectionLeft = std::min(m_selectionStart, m_selectionEnd);
        auto selectionRight = std::max(m_selectionStart, m_selectionEnd);

        if (isControlPressed) {
            if (key == InputKey::A) {
                m_selectionStart = 0;
                m_selectionEnd = m_text.size();
                continue;
            }

            if (key == InputKey::C || key == InputKey::X) {
                if (selectionLeft != selectionRight)
                    toClipboardUtf8(m_text.substr(selectionLeft, selectionRight - selectionLeft).toString());
            }

            if (key == InputKey::V) {
                auto clipboardText = fromClipboardUtf8();
                if (!clipboardText.empty()) {
                    auto newText = m_text;
                    newText.erase(selectionLeft, selectionRight);
                    newText.insert(selectionLeft, clipboardText);
                    m_text = filterText(newText);
                    setCursor(selectionLeft + Utf8Text(clipboardText).size());
                }
            }
        }

        if (key == InputKey::BackSpace || key == InputKey::Delete
            || (key == InputKey::X && isControlPressed)) {
            if (selectionLeft == selectionRight) {
                if (key == InputKey::BackSpace) {
                    if (selectionRight == 0)
                        continue;
                    else
                        selectionLeft = selectionRight - 1;
                }
                if (key == InputKey::Delete) {
                    if (selectionLeft == m_text.size())
                        continue;
                    else
                        selectionRight = selectionLeft + 1;
                }
            }

            if (selectionLeft != selectionRight) {
                auto newText = m_text;
                newText.erase(selectionLeft, selectionRight);
                m_text = filterText(newText);

                setCursor(selectionLeft);
            }
        }

        switch (key) {
        case InputKey::Enter: setSelectionState(SelectionState::None); break;
        case InputKey::Left: moveCursor(-1); break;
        case InputKey::Right: moveCursor(+1); break;
        case InputKey::Home: setCursor(0); break;
        case InputKey::End: setCursor(m_text.size()); break;
        }
    }

    for (auto ch : input.chars) {
        ch = filter(ch);
        if (ch == 0)
            continue;
        sf::String str(ch);
        processPrintable(toString(str.toAnsiString(LOCALE)));
        anyChange = true;
    }

    if (anyChange) {
        m_skin->setText(m_text.toString());
        updateSkin();
    }
}

void TextBox::processPrintable(const std::string& localStr)
{
    auto selectionLeft = std::min(m_selectionStart, m_selectionEnd);
    auto selectionRight = std::max(m_selectionStart, m_selectionEnd);
    auto newText = m_text;
    newText.erase(selectionLeft, selectionRight);
    newText.insert(selectionLeft, convertToUtf8(localStr));
    m_text = filterText(newText);

    setCursor(selectionLeft + 1);
}

void TextBox::processMouse(const InputRegister& input)
{
    if (input.keys.isPressed(InputKey::MouseLeft)) {
        float x = (fullTransform().inversed() * input.mousePosition()).x;
        if (input.keys.isJustPressed(InputKey::MouseLeft)) {
            setCursor(calcCharIndex(x));
            m_timer.start();
			m_timer.setPeriod(m_skin->shiftPeriod());
        } else {
            if (!m_timer.shift())
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
