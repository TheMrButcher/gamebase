#include <stdafx.h>
#include <gamebase/engine/TextEdit.h>
#include <gamebase/engine/TextEditCursor.h>
#include <gamebase/engine/AutoLengthTextFilter.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <locale>

namespace gamebase {

namespace {
bool isCharStartLess(const CharPosition& charPos, float x)
{
    return charPos.position.bottomLeft.x < x;
}
}

TextEdit::TextEdit(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<TextEditSkin>& skin,
    const std::shared_ptr<ITextFilter>& textFilter)
    : OffsettedPosition(position)
    , FindableGeometry(this, skin->geometry())
    , Drawable(this)
    , m_skin(skin)
    , m_textFilter(textFilter
        ? textFilter : std::make_shared<AutoLengthTextFilter>(skin.get()))
    , m_selectionStart(0)
    , m_selectionEnd(0)
{}

void TextEdit::setText(const std::string& text)
{
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    m_text = text;
    m_skin->setText(m_text);
    m_skin->loadResources();
}

void TextEdit::setSelectionState(SelectionState::Enum state)
{
    if (state == SelectionState::Pressed)
        state = SelectionState::Selected;
    if (m_selectionState == SelectionState::Selected && state == SelectionState::MouseOn)
        return;
    if (m_selectionState == SelectionState::Selected && state != m_selectionState)
        m_skin->setSelection(0, 0);
    m_selectionState = state;
    m_skin->setSelectionState(state);
}

void TextEdit::processInput(const InputRegister& input)
{
    const auto& keys = input.keys.signals();
    for (auto it = keys.begin(); it != keys.end(); ++it)
        processKey(*it);

    const auto& specialKeys = input.specialKeys.signals();
    for (auto it = specialKeys.begin(); it != specialKeys.end(); ++it)
        processSpecialKey(*it);

    processMouse(input);
}

void TextEdit::registerObject(PropertiesRegisterBuilder* builder)
{
    registerSelectionState(builder);
    builder->registerObject("skin", m_skin.get());
    builder->registerProperty("text", &m_text,
        std::bind(&TextEdit::setText, this, std::placeholders::_1));
}

void TextEdit::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin;
}

IObject* deserializeTextEdit(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<TextEditSkin>, skin);
    return new TextEdit(position, skin);
}

REGISTER_CLASS(TextEdit);

void TextEdit::processKey(char key)
{
    std::locale loc("");
    bool anyChange = false;
    auto selectionLeft = std::min(m_selectionStart, m_selectionEnd);
    auto selectionRight = std::max(m_selectionStart, m_selectionEnd);

    if (std::isprint(key, loc)) {
        std::string prefix = m_text.substr(0, selectionLeft);
        std::string suffix = m_text.substr(selectionRight);
        m_text = m_textFilter->filter(prefix + key + suffix);

        setCursor(selectionLeft + 1);
        anyChange = true;
    }

    if (key == 8 || key == 127) {
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

        std::string prefix = m_text.substr(0, selectionLeft);
        std::string suffix = m_text.substr(selectionRight);
        m_text = m_textFilter->filter(prefix + suffix);

        setCursor(selectionLeft);
        anyChange = true;
    }
        
    if (anyChange) {
        m_skin->setText(m_text);
        m_skin->setSelection(m_selectionStart, m_selectionEnd);
        m_skin->loadResources();
    }
}
    
void TextEdit::processSpecialKey(SpecialKey::Enum key)
{
    switch (key) {
        case SpecialKey::Left: moveCursor(-1); break;
        case SpecialKey::Right: moveCursor(+1); break;
        case SpecialKey::Home: setCursor(0); break;
        case SpecialKey::End: setCursor(m_text.size()); break;
        default: return;
    }

    m_skin->setSelection(m_selectionStart, m_selectionEnd);
    m_skin->loadResources();
}

void TextEdit::processMouse(const InputRegister& input)
{
    if (input.mouseButtons.isJustPressed(MouseButton::Left)
        || (input.mouseButtons.isPressed(MouseButton::Left) && input.changedPosition())) {
        float x = (fullTransform().inversed() * input.mousePosition()).x;
        if (input.mouseButtons.isJustPressed(MouseButton::Left)) {
            setCursor(calcCharIndex(x));
        } else {
            size_t newEnd = calcCharIndex(x);
            if (newEnd == m_selectionEnd)
                return;
            m_selectionEnd = newEnd;
        }

        m_skin->setSelection(m_selectionStart, m_selectionEnd);
        m_skin->loadResources();
    }
}

void TextEdit::setCursor(size_t pos)
{
    pos = std::min(pos, m_text.size());
    m_selectionStart = pos;
    m_selectionEnd = pos;
}

void TextEdit::moveCursor(int shift)
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

size_t TextEdit::calcCharIndex(float x)
{
    const auto& textGeom = m_skin->textGeometry();
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

void TextEditCursor::serialize(Serializer& s) const
{
    s << "width" << m_width << "color" << m_rect.color();
}

IObject* deserializeTextEditCursor(Deserializer& deserializer)
{
    DESERIALIZE(float, width);
    DESERIALIZE(Color, color);
    auto* result = new TextEditCursor();
    result->setWidth(width);
    result->setColor(color);
    return result;
}

REGISTER_CLASS(TextEditCursor);

}
