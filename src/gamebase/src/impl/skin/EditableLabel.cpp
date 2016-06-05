#include <stdafx.h>
#include <gamebase/impl/skin/tools/EditableLabel.h>
#include <gamebase/impl/graphics/TextureProgram.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

EditableLabel::EditableLabel(const std::shared_ptr<IRelativeBox>& box)
    : Drawable(this)
    , m_box(box)
    , m_offsetX(0)
    , m_isLimited(false)
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

    Vec2 textOffset(m_offsetX, 0);
    size_t firstVisible = size_t(-1);
    size_t lastVisible = 0;
    m_visibleTextGeom.clear();
    auto rect = m_box->get();
    for (size_t i = 0; i + 1 != m_textGeom.size(); ++i) {
        auto box = m_textGeom[i].position;
        box.topRight.x = m_textGeom[i + 1].position.bottomLeft.x;
        box.move(textOffset);
        if (box.intersectWith(rect) == box) {
            if (i < firstVisible)
                firstVisible = i;
            m_visibleTextGeom.push_back(m_textGeom[i]);
            m_visibleTextGeom.back().position.move(textOffset);
            lastVisible = i;
        } else {
            if (!m_visibleTextGeom.empty())
                break;
        }
    }
    if (m_selection.first < m_selection.second) {
        size_t curStart = firstVisible > m_selection.first
            ? firstVisible : m_selection.first;
        size_t curEnd = lastVisible + 1 < m_selection.second
            ? lastVisible + 1 : m_selection.second;
        BoundingBox box(
            m_textGeom.at(curStart).position.bottomLeft,
            Vec2(
                m_textGeom.at(curEnd).position.bottomLeft.x,
                m_textGeom.at(curEnd).position.topRight.y));
        box.move(textOffset);
        m_selectionRect.setBox(box);
        m_selectionRect.loadResources();
    }
    m_buffers = createTextGeometryBuffers(m_visibleTextGeom);
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
        << "selectionColor" << m_selectionRect.color() << "isLimited" << m_isLimited;
}

std::unique_ptr<IObject> deserializeEditableLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Color, color);
    DESERIALIZE(FontDesc, font);
    DESERIALIZE(Color, selectionColor);
    DESERIALIZE_OPT(bool, isLimited, false);
    std::unique_ptr<EditableLabel> result(new EditableLabel(box));
    result->setColor(color);
    result->setFont(font);
    result->setSelectionColor(selectionColor);
    result->setLimited(isLimited);
    return std::move(result);
}

REGISTER_CLASS(EditableLabel);

void EditableLabel::updateTextGeometry()
{
    auto rect = m_box->get();
    auto alignedText = alignText(m_text + ' ', m_alignProps, rect);
    m_textGeom = createTextGeometry(alignedText, m_font.get());
    auto lastCharPosition = m_textGeom.back();
    bool removeLast = false;
    float frontX = m_textGeom.front().position.bottomLeft.x;
    if (!m_isLimited)
        return;
    while (!m_textGeom.empty()
        && m_textGeom.back().position.bottomLeft.x > rect.topRight.x) {
        removeLast = true;
        m_textGeom.pop_back();
        m_text.pop_back();
    }
    if (removeLast) {
        float x = m_textGeom.empty()
            ? frontX
            : m_textGeom.back().position.bottomLeft.x;
        float w = lastCharPosition.position.width();
        lastCharPosition.position.bottomLeft.x = x;
        lastCharPosition.position.topRight.x = x + w;
        if (m_textGeom.empty()) {
            m_textGeom.push_back(lastCharPosition);
        } else {
            m_textGeom.back() = lastCharPosition;
            m_text.pop_back();
        }
    }
}

} }
