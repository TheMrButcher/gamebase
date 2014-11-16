#include <stdafx.h>
#include <gamebase/engine/ObjectsCollection.h>

namespace gamebase {

ObjectsCollection::ObjectsCollection(const std::shared_ptr<IObject>& mainObject)
    : m_position(nullptr)
    , m_mainDrawable(nullptr)
{
    if (mainObject) {
        m_objects.push_back(mainObject);
        if (auto movable = dynamic_cast<IMovable*>(mainObject.get()))
            m_movableObjects.push_back(movable);
        m_position = dynamic_cast<IPositionable*>(mainObject.get());
        m_mainDrawable = dynamic_cast<IDrawable*>(mainObject.get());
    }
}

void ObjectsCollection::addChild(const std::shared_ptr<IObject>& object)
{
    m_objects.push_back(object);
    if (auto movable = dynamic_cast<IMovable*>(object.get()))
        m_movableObjects.push_back(movable);
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        m_drawableObjects.push_back(drawable);
}

Transform2 ObjectsCollection::position() const
{
    return m_position ? m_position->position() : Transform2();
}

Transform2 ObjectsCollection::transform() const
{
    return m_position ? m_position->transform() : Transform2();
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

}
