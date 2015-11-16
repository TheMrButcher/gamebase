#include <stdafx.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

CanvasLayout::CanvasLayout(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_box(box)
    , m_nextID(0)
{
    m_list.setParentPosition(this);
}

int CanvasLayout::addObject(const std::shared_ptr<IObject>& obj)
{
    int id = m_nextID++;
    m_objects[id] = obj;
    m_list.addObject(obj);
    loadIfNeeded(m_box, obj.get());
    return id;
}

void CanvasLayout::replaceObject(int id, const std::shared_ptr<IObject>& obj)
{
    m_nextID = std::max(m_nextID, id + 1);
    m_objects[id] = obj;
    refill();
    loadIfNeeded(m_box, obj.get());
}

void CanvasLayout::removeObject(int id)
{
    m_objects.erase(id);
    refill();
}

void CanvasLayout::update()
{
    m_list.setBox(box());
    m_list.loadResources();
}

void CanvasLayout::clear()
{
    m_list.clear();
    m_objects.clear();
    m_nextID = 0;
}

void CanvasLayout::setAssociatedSelectable(ISelectable* selectable)
{
    m_list.setAssociatedSelectable(selectable);
}

std::shared_ptr<IObject> CanvasLayout::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    return m_list.findChildByPoint(position().inversed() * point);
}

void CanvasLayout::loadResources()
{
    m_list.loadResources();
}

void CanvasLayout::drawAt(const Transform2& position) const
{
    m_list.draw(position);
}

void CanvasLayout::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    m_list.setBox(box());
    setPositionBoxes(allowedBox, box());
}

void CanvasLayout::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("objects", &m_list);
}

void CanvasLayout::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "objects" << m_objects;
}

std::unique_ptr<IObject> deserializeCanvasLayout(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    std::unique_ptr<CanvasLayout> result(new CanvasLayout(box, position));
    deserializer >> "objects" >> result->m_objects;
    result->m_nextID = result->m_objects.empty() ? 0 : (result->m_objects.rbegin()->first + 1);
    result->refill();
    return std::move(result);
}

REGISTER_CLASS(CanvasLayout);

void CanvasLayout::refill()
{
    m_list.clear();
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        m_list.addObject(it->second);
}

}
