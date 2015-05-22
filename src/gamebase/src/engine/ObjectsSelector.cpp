#include <stdafx.h>
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

ObjectsSelector::ObjectsSelector(const std::shared_ptr<IObject>& mainObject)
    : m_position(nullptr)
    , m_currentObjectID(-1)
{
    if (mainObject) {
        m_mainObj = mainObject;
        m_position = dynamic_cast<IPositionable*>(mainObject.get());
        if (m_position)
            m_position->setParentPosition(m_parentPosition);
    }
}

void ObjectsSelector::addObject(int id, const std::shared_ptr<IObject>& object)
{
    m_objects[id] = object;
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    if (auto movable = dynamic_cast<IMovable*>(object.get()))
        m_movableObjects[id] = movable;
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        m_drawableObjects[id] = drawable;
    if (auto findable = dynamic_cast<IFindable*>(object.get()))
        m_findableObjects[id] = findable;
    if (m_registerBuilder)
        m_registerBuilder->registerObject(object.get());
}

void ObjectsSelector::select(int id)
{
    m_currentObjectID = id;
}

Transform2 ObjectsSelector::position() const
{
    return m_position ? m_position->position() : Transform2();
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

void ObjectsSelector::move()
{
    auto it = m_movableObjects.find(m_currentObjectID);
    if (it != m_movableObjects.end())
        it->second->move();
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

void ObjectsSelector::registerObject(PropertiesRegisterBuilder* builder)
{
    m_registerBuilder.reset(new PropertiesRegisterBuilder(*builder));
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        builder->registerObject(it->second.get());
    builder->registerProperty("currentID", &m_currentObjectID);
}

void ObjectsSelector::serialize(Serializer& s) const
{
    s << "objects" << m_objects << "mainObj" << m_mainObj << "currentID" << m_currentObjectID;
}

std::unique_ptr<IObject> deserializeObjectsSelector(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> IdToObj;
    DESERIALIZE(std::shared_ptr<IObject>, mainObj);
    DESERIALIZE(IdToObj, objects);
    DESERIALIZE(int, currentID);
    std::unique_ptr<ObjectsSelector> result(new ObjectsSelector(mainObj));
    for (auto it = objects.begin(); it != objects.end(); ++it)
        result->addObject(it->first, it->second);
    result->select(currentID);
    return std::move(result);
}

REGISTER_CLASS(ObjectsSelector);

}
