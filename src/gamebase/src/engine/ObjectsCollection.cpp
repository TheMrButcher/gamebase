#include <stdafx.h>
#include <gamebase/engine/ObjectsCollection.h>

namespace gamebase {

ObjectsCollection::ObjectsCollection(const std::shared_ptr<IObject>& mainObject)
    : m_position(nullptr)
    , m_mainDrawable(nullptr)
    , m_mainFindable(nullptr)
{
    if (mainObject) {
        m_objects.push_back(mainObject);
        setMainObject(mainObject.get());
    }
}

ObjectsCollection::ObjectsCollection(IObject* mainObject)
    : m_position(nullptr)
    , m_mainDrawable(nullptr)
    , m_mainFindable(nullptr)
{
    if (mainObject)
        setMainObject(mainObject);
}

void ObjectsCollection::addChild(const std::shared_ptr<IObject>& object)
{
    m_objects.push_back(object);
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    if (auto movable = dynamic_cast<IMovable*>(object.get()))
        m_movableObjects.push_back(movable);
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        m_drawableObjects.push_back(drawable);
    if (auto findable = dynamic_cast<IFindable*>(object.get()))
        m_findableObjects.push_back(findable);
}

Transform2 ObjectsCollection::position() const
{
    return m_position ? m_position->position() : Transform2();
}

Transform2 ObjectsCollection::transform() const
{
    return m_position ? m_position->transform() : Transform2();
}

void ObjectsCollection::setParentPosition(const IPositionable* parent)
{
    IPositionable::setParentPosition(parent);
    if (m_position)
        m_position->setParentPosition(m_parentPosition);
}

IObject* ObjectsCollection::find(
    const Vec2& point, const Transform2& globalPosition)
{
    auto pos = position() * globalPosition;
    for (auto it = m_findableObjects.begin(); it != m_findableObjects.end(); ++it)
        if (auto obj = (*it)->find(point, pos))
            return obj;

    if (m_mainFindable)
        return m_mainFindable->find(point, globalPosition);
    return nullptr;
}

void ObjectsCollection::move(float time)
{
    for (auto it = m_movableObjects.begin(); it != m_movableObjects.end(); ++it)
        (*it)->move(time);
}

void ObjectsCollection::loadResources()
{
    if (m_mainDrawable)
        m_mainDrawable->loadResources();

    for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
        (*it)->loadResources();
}

void ObjectsCollection::draw(const Transform2& globalPosition) const
{
    if (m_mainDrawable)
        m_mainDrawable->draw(globalPosition);

    auto pos = position() * globalPosition;
    for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
        (*it)->draw(pos);
}

void ObjectsCollection::setMainObject(IObject* mainObject)
{
    if (auto movable = dynamic_cast<IMovable*>(mainObject))
        m_movableObjects.push_back(movable);
    m_position = dynamic_cast<IPositionable*>(mainObject);
    if (m_position)
        m_position->setParentPosition(m_parentPosition);
    m_mainDrawable = dynamic_cast<IDrawable*>(mainObject);
    m_mainFindable = dynamic_cast<IFindable*>(mainObject);
}

}
