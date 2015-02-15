#include <stdafx.h>
#include <gamebase/engine/ObjectsSelector.h>

namespace gamebase {

ObjectsSelector::ObjectsSelector(const std::shared_ptr<IObject>& mainObject)
    : m_position(nullptr)
    , m_currentObjectID(-1)
{
    if (mainObject) {
        m_objects.push_back(mainObject);
        m_position = dynamic_cast<IPositionable*>(mainObject.get());
        if (m_position)
            m_position->setParentPosition(m_parentPosition);
    }
}

void ObjectsSelector::addChild(int id, const std::shared_ptr<IObject>& object)
{
    m_objects.push_back(object);
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    if (auto movable = dynamic_cast<IMovable*>(object.get()))
        m_movableObjects[id] = movable;
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        m_drawableObjects[id] = drawable;
    if (auto findable = dynamic_cast<IFindable*>(object.get()))
        m_findableObjects[id] = findable;
}

void ObjectsSelector::select(int id)
{
    m_currentObjectID = id;
}

Transform2 ObjectsSelector::position() const
{
    return m_position ? m_position->position() : Transform2();
}

Transform2 ObjectsSelector::transform() const
{
    return m_position ? m_position->transform() : Transform2();
}

void ObjectsSelector::setParentPosition(const IPositionable* parent)
{
    IPositionable::setParentPosition(parent);
    if (m_position)
        m_position->setParentPosition(m_parentPosition);
}

IObject* ObjectsSelector::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!isVisible())
        return nullptr;

    auto it = m_findableObjects.find(m_currentObjectID);
    if (it != m_findableObjects.end())
        return it->second->find(point, position() * globalPosition);
    return nullptr;
}

void ObjectsSelector::move(float time)
{
    auto it = m_movableObjects.find(m_currentObjectID);
    if (it != m_movableObjects.end())
        it->second->move(time);
}

void ObjectsSelector::loadResources()
{
    for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
        it->second->loadResources();
}

void ObjectsSelector::drawAt(const Transform2& position) const
{
    auto it = m_drawableObjects.find(m_currentObjectID);
    if (it != m_drawableObjects.end())
        it->second->draw(position);
}

void ObjectsSelector::setBox(const BoundingBox& allowedBox)
{
    for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
        it->second->setBox(allowedBox);
}

BoundingBox ObjectsSelector::box() const
{
    BoundingBox result;
    for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
        result.enlarge(it->second->box());
    return result;
}

}
