#include <stdafx.h>
#include <gamebase/impl/tools/ObjectsSelector.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ObjectsSelector::ObjectsSelector(const IPositionable* position)
    : Drawable(position)
    , m_position(position)
    , m_currentObjectID(-1)
{}

void ObjectsSelector::insertObject(int id, const std::shared_ptr<IObject>& object)
{
    {
        auto& value = m_objects[id];
        if (value && m_registerBuilder)
            m_register.remove(value.get());
        value = object;
    }

    auto& objDesc = m_objDescs[id];
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        objDesc.positionable = positionable;
    if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
        objDesc.drawable = drawable;
    if (auto findable = dynamic_cast<IFindable*>(object.get()))
        objDesc.findable = findable;
    if (m_registerBuilder)
        m_registerBuilder->registerObject(object.get());
}

void ObjectsSelector::removeObject(int id)
{
    if (m_currentObjectID == id)
        m_currentObjectID = -1;
    auto it = m_objects.find(id);
    if (it == m_objects.end())
        return;
    m_register.remove(it->second.get());
    m_objects.erase(it);
    m_objDescs.erase(id);
}

void ObjectsSelector::clear()
{
    m_objects.clear();
    m_objDescs.clear();
    m_register.clear();
    m_currentObjectID = -1;
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
    auto it = m_objDescs.find(m_currentObjectID);
    if (it != m_objDescs.end()) {
        auto* findable = it->second.findable;
        if (!findable)
            return nullptr;
        if (auto obj = findable->findChildByPoint(transformedPoint))
            return obj;
        if (findable->isSelectableByPoint(transformedPoint))
            return m_objects.at(it->first);
    }
    return nullptr;
}

void ObjectsSelector::loadResources()
{
    for (auto it = m_objDescs.begin(); it != m_objDescs.end(); ++it)
        it->second.drawable->loadResources();
}

void ObjectsSelector::drawAt(const Transform2& position) const
{
    auto it = m_objDescs.find(m_currentObjectID);
    if (it != m_objDescs.end())
        it->second.drawable->draw(position);
}

void ObjectsSelector::setBox(const BoundingBox& allowedBox)
{
    for (auto it = m_objDescs.begin(); it != m_objDescs.end(); ++it)
        it->second.drawable->setBox(allowedBox);
}

BoundingBox ObjectsSelector::box() const
{
    BoundingBox result;
    for (auto it = m_objDescs.begin(); it != m_objDescs.end(); ++it) {
        if (it->second.drawable) {
            auto box = it->second.drawable->box();
            if (it->second.positionable)
                box = box.transformed(it->second.positionable->position());
            result.enlarge(box);
        }
    }
    return result;
}

void ObjectsSelector::registerObject(PropertiesRegisterBuilder* builder)
{
    m_registerBuilder.reset(new PropertiesRegisterBuilder(*builder));
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        builder->registerObject(it->second.get());
    builder->registerProperty("cur", &m_currentObjectID,
        std::bind(&ObjectsSelector::select, this, std::placeholders::_1));
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
        obj->insertObject(it->first, it->second);
    obj->select(currentID);
}

} }
