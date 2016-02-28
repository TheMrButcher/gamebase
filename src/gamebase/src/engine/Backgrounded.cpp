#include <stdafx.h>
#include <gamebase/engine/Backgrounded.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

Backgrounded::Backgrounded(
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_mainDrawable(nullptr)
    , m_mainFindable(nullptr)
    , m_backgroundDrawable(nullptr)
{}

void Backgrounded::setMainObject(const std::shared_ptr<IObject>& obj)
{
    m_mainObj = obj;
    if (auto positionable = dynamic_cast<IPositionable*>(m_mainObj.get()))
        positionable->setParentPosition(this);
    m_mainDrawable = dynamic_cast<Drawable*>(m_mainObj.get());
    m_mainFindable = dynamic_cast<IFindable*>(m_mainObj.get());
}

void Backgrounded::setBackground(const std::shared_ptr<IObject>& obj)
{
    m_backgroundObj = obj;
    if (auto positionable = dynamic_cast<OffsettedPosition*>(m_backgroundObj.get())) {
        positionable->setParentPosition(this);
        positionable->setRelativeOffset(std::make_shared<AligningOffset>(
            HorAlign::Center, VertAlign::Center));
    }
    m_backgroundDrawable = dynamic_cast<Drawable*>(m_backgroundObj.get());
}

std::shared_ptr<IObject> Backgrounded::findChildByPoint(const Vec2& point) const
{
    if (!isVisible() || !m_mainFindable)
        return false;
    return m_mainFindable->findChildByPoint(position().inversed() * point);
}

void Backgrounded::loadResources()
{
    if (m_mainDrawable)
        m_mainDrawable->loadResources();
    if (m_backgroundDrawable)
        m_backgroundDrawable->loadResources();
}

void Backgrounded::drawAt(const Transform2& position) const
{
    if (m_backgroundDrawable)
        m_backgroundDrawable->draw(position);
    if (m_mainDrawable)
        m_mainDrawable->draw(position);
}

void Backgrounded::setBox(const BoundingBox& allowedBox)
{
    m_curBox = allowedBox;
    if (m_mainDrawable) {
        m_mainDrawable->setBox(m_curBox);
        m_curBox = m_mainDrawable->movedBox();
    }
    if (m_backgroundDrawable) {
        m_backgroundDrawable->setBox(m_curBox);
        m_curBox = m_backgroundDrawable->movedBox();
    }
    setPositionBoxes(allowedBox, m_curBox);
}

void Backgrounded::registerObject(PropertiesRegisterBuilder* builder)
{
    if (m_mainObj)
        builder->registerObject(m_mainObj.get());
    if (m_backgroundObj)
        builder->registerObject(m_backgroundObj.get());
}

void Backgrounded::serialize(Serializer& s) const
{
    s << "position" << m_offset << "obj" << m_mainObj << "background" << m_backgroundObj;
}

std::unique_ptr<IObject> deserializeBackgrounded(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<IObject>, obj);
    DESERIALIZE(std::shared_ptr<IObject>, background);
    std::unique_ptr<Backgrounded> result(new Backgrounded(position));
    result->setMainObject(obj);
    result->setBackground(background);
    return std::move(result);
}

REGISTER_CLASS(Backgrounded);

}
