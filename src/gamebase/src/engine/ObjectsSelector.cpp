#include <stdafx.h>
#include <gamebase/engine/ObjectsSelector.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

ObjectsSelector::ObjectsSelector(const IPositionable* position)
    : Drawable(position)
    , m_position(position)
    , m_currentObjectID(-1)
{}

void ObjectsSelector::addObject(int id, const std::shared_ptr<IObject>& object)
{
    m_objects[id] = object;
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        m_drawableObjects[id] = drawable;
    if (auto findable = dynamic_cast<IFindable*>(object.get()))
        m_findableObjects[id] = findable;
    if (m_registerBuilder)
        m_registerBuilder->registerObject(object.get());
}

void ObjectsSelector::removeObject(int id)
{
    m_objects.erase(id);
    m_drawableObjects.erase(id);
    m_findableObjects.erase(id);
    if (m_currentObjectID == id)
        m_currentObjectID = -1;

    // ToDo:
    //m_register.remove();
}

void ObjectsSelector::select(int id)
{
    m_currentObjectID = id;
}

std::shared_ptr<IObject> ObjectsSelector::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;

    Vec2 transformedPoint = m_position ? m_position->position().inversed() * point : point;
    auto it = m_findableObjects.find(m_currentObjectID);
    if (it != m_findableObjects.end()) {
        auto* findable = it->second;
        if (auto obj = findable->findChildByPoint(transformedPoint))
            return obj;
        if (findable->isSelectableByPoint(transformedPoint))
            return m_objects.at(it->first);
    }
    return nullptr;
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
    s << "objects" << m_objects << "currentID" << m_currentObjectID;
}

std::unique_ptr<IObject> deserializeObjectsSelector(Deserializer& deserializer)
{
    std::unique_ptr<ObjectsSelector> result(new ObjectsSelector());
    deserializeObjectsSelectorElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(ObjectsSelector);

void deserializeObjectsSelectorElements(
    Deserializer& deserializer, ObjectsSelector* obj)
{
    typedef std::map<int, std::shared_ptr<IObject>> IdToObj;
    DESERIALIZE(IdToObj, objects);
    DESERIALIZE(int, currentID);
    for (auto it = objects.begin(); it != objects.end(); ++it)
        obj->addObject(it->first, it->second);
    obj->select(currentID);
}

}
