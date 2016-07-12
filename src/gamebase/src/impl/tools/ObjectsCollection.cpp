#include <stdafx.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ObjectsCollection::ObjectsCollection(IPositionable* position)
    : Drawable(this)
    , m_position(position)
    , m_associatedSelectable(nullptr)
{}

void ObjectsCollection::addObject(const std::shared_ptr<IObject>& object)
{
    m_objectDescs.push_back(registerObject(object));
    m_objects.push_back(object);
}

void ObjectsCollection::replaceObject(int id, const std::shared_ptr<IObject>& object)
{
    if (id < 0 || static_cast<size_t>(id) >= m_objects.size())
        THROW_EX() << "Index=" << id << " of object to replace is out of bounds: "
            "[0; " << m_objects.size() << ")";
    if (m_registerBuilder)
        m_register.remove(m_objects[id].get());
    if (auto selectableObj = dynamic_cast<ISelectable*>(m_objects[id].get()))
        selectableObj->setAssociatedSelectable(nullptr);
    m_objectDescs[id] = registerObject(object);
    m_objects[id] = object;
}

bool ObjectsCollection::removeObject(int id)
{
    if (id < 0 || static_cast<size_t>(id) >= m_objects.size())
        return false;
    if (m_registerBuilder)
        m_register.remove(m_objects[id].get());
    if (auto selectableObj = dynamic_cast<ISelectable*>(m_objects[id].get()))
        selectableObj->setAssociatedSelectable(nullptr);
    m_objectDescs.erase(m_objectDescs.begin() + id);
    m_objects.erase(m_objects.begin() + id);
    return true;
}

int ObjectsCollection::findObject(IObject* obj) const
{
    int i = 0;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it, ++i)
        if (it->get() == obj)
            return i;
    return -1;
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

void ObjectsCollection::move(float time)
{
    for (auto it = m_objectDescs.begin(); it != m_objectDescs.end(); ++it) {
        if (it->movable)
            it->movable->move(time);
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
        if (it->drawable) {
            auto box = it->drawable->box();
            if (it->positionable)
                box = box.transformed(it->positionable->position());
            result.enlarge(box);
        }
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

ObjectsCollection::ObjectDesc ObjectsCollection::registerObject(const std::shared_ptr<IObject>& object)
{
    ObjectDesc desc;
    if (auto positionable = dynamic_cast<IPositionable*>(object.get())) {
        desc.positionable = positionable;
        positionable->setParentPosition(this);
    }
    desc.movable = dynamic_cast<IMovable*>(object.get());
    desc.drawable = dynamic_cast<IDrawable*>(object.get());
    desc.findable = dynamic_cast<IFindable*>(object.get());
    if (m_associatedSelectable) {
        if (auto selectableObj = dynamic_cast<ISelectable*>(object.get()))
            selectableObj->setAssociatedSelectable(m_associatedSelectable);
    }
    if (m_registerBuilder)
        m_registerBuilder->registerObject(object.get());
    return desc;
}

} }
