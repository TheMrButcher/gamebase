#include <stdafx.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/FixedBox.h>
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
    , m_adjustment(Adjustment::None)
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

void CanvasLayout::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    m_nextID = std::max(m_nextID, id + 1);
    m_objects[id] = obj;
    refill();
    loadIfNeeded(m_box, obj.get());
}

void CanvasLayout::insertObjects(
    const std::map<int, std::shared_ptr<IObject>>& objects)
{
    if (objects.empty())
        return;

    if (m_objects.empty()) {
        m_objects = objects;
    } else {
        m_objects.insert(objects.begin(), objects.end());
    }

    m_nextID = std::max(m_nextID, objects.rbegin()->first + 1);
    refill();

    if (m_box->isValid()) {
        for (auto it = objects.begin(); it != objects.end(); ++it)
            loadIfNeededNoCheck(m_box, it->second.get());
    }
}

void CanvasLayout::removeObject(int id)
{
    m_objects.erase(id);
    refill();
}

void CanvasLayout::removeObject(IObject* obj)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second.get() == obj) {
            removeObject(it->first);
            return;
        }
    }       
}

void CanvasLayout::update()
{
    if (m_adjustment == Adjustment::None) {
        m_list.setBox(m_box->get());
    } else {
        setBox(m_parentBox);
    }
    loadResources();
}

void CanvasLayout::clear()
{
    m_list.clear();
    m_objects.clear();
    m_nextID = 0;
}

void CanvasLayout::setFixedBox(float width, float height)
{
    m_box = std::make_shared<FixedBox>(width, height);
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
    m_parentBox = allowedBox;
    m_box->setParentBox(allowedBox);
    m_list.setBox(m_box->get());
    if (m_adjustment == Adjustment::None) {
        m_curBox = m_box->get();
    } else {
        m_curBox = m_list.box();
        if (!m_curBox.isValid() || m_adjustment == Adjustment::ToFitContentAndArea)
            m_curBox.enlarge(m_box->get());
    }
    setPositionBoxes(allowedBox, box());
}

void CanvasLayout::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("objects", &m_list);
}

void CanvasLayout::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "objects" << m_objects << "adjustment" << m_adjustment;
}

std::unique_ptr<IObject> deserializeCanvasLayout(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Objects, objects);
    DESERIALIZE_OPT(Adjustment::Enum, adjustment, Adjustment::None);
    std::unique_ptr<CanvasLayout> result(new CanvasLayout(box, position));
    result->insertObjects(objects);
    result->setAdjustment(adjustment);
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
