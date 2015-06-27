#include <stdafx.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

ObjectsCollection::ObjectsCollection(IPositionable* position)
    : Drawable(this)
    , m_position(position)
    , m_associatedSelectable(nullptr)
{}

void ObjectsCollection::addObject(const std::shared_ptr<IObject>& object)
{
    ObjectDesc desc;
    if (auto positionable = dynamic_cast<IPositionable*>(object.get()))
        positionable->setParentPosition(this);
    desc.movable = dynamic_cast<IMovable*>(object.get());
    desc.drawable = dynamic_cast<IDrawable*>(object.get());
    desc.findable = dynamic_cast<IFindable*>(object.get());
    if (m_associatedSelectable) {
        if (auto selectableObj = dynamic_cast<ISelectable*>(object.get()))
            selectableObj->setAssociatedSelectable(m_associatedSelectable);
    }
    if (m_registerBuilder)
        m_registerBuilder->registerObject(object.get());
    m_objectDescs.push_back(desc);
    m_objects.push_back(object);
}

Transform2 ObjectsCollection::position() const
{
    return m_position ? m_position->position() : Transform2();
}

void ObjectsCollection::setParentPosition(const IPositionable* parent)
{
    IPositionable::setParentPosition(parent);
    if (m_position)
        m_position->setParentPosition(m_parentPosition);
}

std::shared_ptr<IObject> ObjectsCollection::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;

    Vec2 transformedPoint = m_position ? m_position->position().inversed() * point : point;
    size_t i = m_objectDescs.size() - 1;
    for (auto it = m_objectDescs.rbegin(); it != m_objectDescs.rend(); ++it, --i) {
        if (!it->findable)
            continue;
        if (auto obj = it->findable->findChildByPoint(transformedPoint))
            return obj;
        if (it->findable->isSelectableByPoint(transformedPoint))
            return m_objects.at(i);
    }
    return nullptr;
}

void ObjectsCollection::move()
{
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->movable)
            it->movable->move();
    }
}

void ObjectsCollection::loadResources()
{
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->drawable)
            it->drawable->loadResources();
    }
}

void ObjectsCollection::drawAt(const Transform2& position) const
{
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->drawable)
            it->drawable->draw(position);
    }
}

void ObjectsCollection::setBox(const BoundingBox& allowedBox)
{
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->drawable)
            it->drawable->setBox(allowedBox);
    }
}

BoundingBox ObjectsCollection::box() const
{
    BoundingBox result;
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->drawable)
            result.enlarge(it->drawable->box());
    }
    return result;
}

void ObjectsCollection::registerObject(PropertiesRegisterBuilder* builder)
{
    m_registerBuilder.reset(new PropertiesRegisterBuilder(*builder));
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        builder->registerObject(it->get());
}

void ObjectsCollection::serialize(Serializer& s) const
{
    s << "objects" << m_objects;
}

std::unique_ptr<IObject> deserializeObjectsCollection(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, objects);
    std::unique_ptr<ObjectsCollection> result(new ObjectsCollection());
    for (auto it = objects.begin(); it != objects.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

REGISTER_CLASS(ObjectsCollection);

void ObjectsCollection::clear()
{
    m_register.clear();
    m_objects.clear();
    m_objectDescs.clear();
}

void ObjectsCollection::setAssociatedSelectable(ISelectable* selectable)
{
    m_associatedSelectable = selectable;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (auto selectableObj = dynamic_cast<ISelectable*>(it->get()))
            selectableObj->setAssociatedSelectable(m_associatedSelectable);
    }
}

}
