/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <unordered_set>

namespace gamebase { namespace impl {

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

bool CanvasLayout::hasObject(int id) const
{
    return m_objects.find(id) != m_objects.end();
}

bool CanvasLayout::hasObject(IObject* obj) const
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second.get() == obj)
            return true;
    }
    return false;
}

int CanvasLayout::addObject(const std::shared_ptr<IObject>& obj)
{
    int id = m_nextID++;
    m_objects[id] = obj;
    m_list.addObject(obj);
    loadIfNeeded(m_box, obj.get());
    updateBox();
    return id;
}

void CanvasLayout::addObjects(const std::vector<std::shared_ptr<IObject>>& objects)
{
    if (objects.empty())
        return;

    for (const auto& obj : objects)
        m_objects[m_nextID++] = obj;

    refill();

    if (m_box->isValid()) {
        for (const auto& obj : objects)
            loadIfNeededNoCheck(m_box, obj.get());
    }
    updateBox();
}

void CanvasLayout::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    if (id >= m_nextID) {
        m_nextID = id + 1;
        m_objects[id] = obj;
        m_list.addObject(obj);
        loadIfNeeded(m_box, obj.get());
        return;
    }
    m_objects[id] = obj;
    refill();
    loadIfNeeded(m_box, obj.get());
    updateBox();
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
    updateBox();
}

void CanvasLayout::removeObject(int id)
{
    m_objects.erase(id);
    refill();
    updateBox();
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

void CanvasLayout::removeObjects(const std::vector<int>& ids)
{
    if (ids.empty())
        return;
    for (auto it = ids.begin(); it != ids.end(); ++it)
        removeObject(*it);
    refill();
    updateBox();
}

void CanvasLayout::removeObjects(const std::vector<IObject*>& objs)
{
    if (objs.empty())
        return;
    if (objs.size() == 1)
        removeObject(objs.front());
    std::unordered_set<IObject*> objsToRemove(objs.begin(), objs.end());
    std::map<int, std::shared_ptr<IObject>> newObjMap;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (objsToRemove.count(it->second.get()) == 0)
            newObjMap[it->first] = it->second;
    }
    newObjMap.swap(m_objects);
    refill();
    updateBox();
}

std::shared_ptr<IObject> CanvasLayout::getIObjectSPtr(IObject* obj) const
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second.get() == obj)
            return it->second;
    }

    THROW_EX() << "Can't find shared_ptr to object by raw pointer";
}

void CanvasLayout::update()
{
	if (!m_parentBox)
        return;
    setBox(*m_parentBox);
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
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
    update();
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
    updateBox();
}

void CanvasLayout::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_list);
}

void CanvasLayout::serialize(Serializer& s) const
{
    s << "adjustment" << m_adjustment << "box" << m_box << "position" << m_offset << "list" << m_list.objects();
}

std::unique_ptr<IObject> deserializeCanvasLayout(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE_OPT(Adjustment::Enum, adjustment, Adjustment::None);
    std::unique_ptr<CanvasLayout> result(new CanvasLayout(box, position));
    if (deserializer.hasMember("objects")) {
        DESERIALIZE(Objects, objects);
        result->insertObjects(objects);
    } else {
        DESERIALIZE(std::vector<std::shared_ptr<IObject>>, list);
        result->addObjects(list);
    }

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

void CanvasLayout::updateBox()
{
    if (!m_parentBox)
        return;
    m_curBox = adjust(m_box->get(), m_list.box(), m_adjustment);
    setPositionBoxes(*m_parentBox, box());
}

} }
