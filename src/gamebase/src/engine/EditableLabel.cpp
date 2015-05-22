#include <stdafx.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/graphics/TextureProgram.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

EditableLabel::EditableLabel(const std::shared_ptr<IRelativeBox>& box)
    : Drawable(this)
    , m_box(box)
{
    m_alignProps.horAlign = HorAlign::Left;
    m_alignProps.vertAlign = VertAlign::Center;
    m_alignProps.enableStacking = false;

    m_selectionRect.setColor(Color(0.8f, 0.8f, 1.0f));
}

void EditableLabel::setText(const std::string& text)
{
    if (m_text == text)
        return;
    m_text = text;
    if (m_font)
        updateTextGeometry();
    else
        m_textGeom.clear();
}

void EditableLabel::loadResources()
{
    m_font = m_alignProps.font.get();
    updateTextGeometry();
    if (m_selection.first < m_selection.second) {
        m_selectionRect.setBox(BoundingBox(
            m_textGeom.at(m_selection.first).position.bottomLeft,
            Vec2(
                m_textGeom.at(m_selection.second).position.bottomLeft.x,
                m_textGeom.at(m_selection.second).position.topRight.y)));
        m_selectionRect.loadResources();
    }
    m_buffers = createTextGeometryBuffers(m_textGeom);
}

void EditableLabel::drawAt(const Transform2& position) const
{
    if (m_text.empty())
        return;

    if (m_selection.first < m_selection.second)
        m_selectionRect.draw(position);

    const TextureProgram& program = textureProgram();
    program.transform = position;
    program.texture = m_font->texture();
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void EditableLabel::serialize(Serializer& s) const
{
    s << "box" << m_box << "color" << m_color << "font" << m_alignProps.font
        << "selectionColor" << m_selectionRect.color();
}

IObject* deserializeEditableLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Color, color);
    DESERIALIZE(FontDesc, font);
    DESERIALIZE(Color, selectionColor);
    auto* result = new EditableLabel(box);
    result->setColor(color);
    result->setFont(font);
    result->setSelectionColor(selectionColor);
    return result;
}

REGISTER_CLASS(EditableLabel);

void EditableLabel::updateTextGeometry()
{
    auto rect = m_box->get();
    auto alignedText = alignText(m_text + ' ', m_alignProps, rect);
    m_textGeom = createTextGeometry(alignedText, m_font.get());
    auto lastCharPosition = m_textGeom.back();
    bool removeLast = false;
    while (!m_textGeom.empty()
        && m_textGeom.back().position.bottomLeft.x > rect.topRight.x) {
        removeLast = true;
        m_textGeom.pop_back();
        m_text.pop_back();
    }
    if (removeLast) {
        float x = m_textGeom.back().position.bottomLeft.x;
        float w = lastCharPosition.position.width();
        lastCharPosition.position.bottomLeft.x = x;
        lastCharPosition.position.topRight.x = x + w;
        m_textGeom.back() = lastCharPosition;
        m_text.pop_back();
    }
}

}
