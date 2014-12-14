#include <stdafx.h>
#include <gamebase/engine/EditableLabel.h>
#include <gamebase/graphics/TextureProgram.h>

namespace gamebase {

EditableLabel::EditableLabel()
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
        m_selectionRect.setRect(BoundingBox(
            m_textGeom.at(m_selection.first).position.bottomLeft,
            Vec2(
                m_textGeom.at(m_selection.second).position.bottomLeft.x,
                m_textGeom.at(m_selection.second).position.topRight.y)));
        m_selectionRect.loadResources();
    }
    m_buffers = createTextGeometryBuffers(m_textGeom);
}

void EditableLabel::draw(const Transform2& globalPosition) const
{
    if (m_text.empty())
        return;

    if (m_selection.first < m_selection.second)
        m_selectionRect.draw(globalPosition);

    const TextureProgram& program = textureProgram();
    program.transform = globalPosition;
    program.texture = m_font->texture();
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void EditableLabel::updateTextGeometry()
{
    auto alignedText = alignText(m_text + ' ', m_alignProps, m_rect);
    m_textGeom = createTextGeometry(alignedText, m_font.get());
    auto lastCharPosition = m_textGeom.back();
    bool removeLast = false;
    while (!m_textGeom.empty()
        && m_textGeom.back().position.bottomLeft.x > m_rect.topRight.x) {
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
