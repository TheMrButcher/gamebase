#include <stdafx.h>
#include <gamebase/engine/AnimatedTextEditSkin.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

AnimatedTextEditSkin::AnimatedTextEditSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : AnimatedObject(box, geom)
    , m_cursorPos(0)
    , m_drawCursor(false)
    , m_loaded(false)
    , m_text(std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()))
{}

void AnimatedTextEditSkin::setSelection(size_t startIndex, size_t endIndex)
{
    if (startIndex == endIndex) {
        m_cursorPos = startIndex;
        m_drawCursor = true;
    } else {
        m_drawCursor = false;
    }
    m_text.setSelection(startIndex, endIndex);
}

void AnimatedTextEditSkin::loadResources()
{
    if (!m_loaded) {
        AnimatedObject::loadResources();
        m_loaded = true;
    }

    m_text.loadResources();
    BoundingBox charBox = m_text.textGeometry().at(m_cursorPos).position;
    m_cursor.setX(charBox.bottomLeft.x);
    m_cursor.setYRange(charBox.bottomLeft.y, charBox.topRight.y);
    m_cursor.loadResources();
}

void AnimatedTextEditSkin::drawAt(const Transform2& position) const
{
    AnimatedObject::drawAt(position);
    m_text.draw(position);
    if (m_selectionState == SelectionState::Selected && m_drawCursor)
        m_cursor.draw(position);
}

void AnimatedTextEditSkin::setBox(const BoundingBox& allowedBox)
{
    AnimatedObject::setBox(allowedBox);
    m_text.setBox(m_box->get());
}

void AnimatedTextEditSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    AnimatedObject::registerObject(builder);
    builder->registerObject("label", &m_text);
    builder->registerObject("cursor", &m_cursor);
}

void AnimatedTextEditSkin::serialize(Serializer& s) const
{
    AnimatedObject::serialize(s);
    s << "label" << m_text << "cursor" << m_cursor;
}

std::unique_ptr<IObject> deserializeAnimatedTextEditSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);

    std::unique_ptr<AnimatedTextEditSkin> result(new AnimatedTextEditSkin(box, geometry));
    deserializeAnimatedObjectElements(deserializer, result.get());
    deserializer >> "label" >> result->label() >> "cursor" >> result->cursor();
    return std::move(result);
}

REGISTER_CLASS(AnimatedTextEditSkin);

}
