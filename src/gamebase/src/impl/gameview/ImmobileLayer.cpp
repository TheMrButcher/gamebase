/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/ImmobileLayer.h>
#include "src/impl/global/GlobalTemporary.h"
#include "LayerHelpers.h"
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ImmobileLayer::ImmobileLayer()
    : m_isGameBoxInited(false)
    , m_needToUpdate(false)
    , m_nextID(0)
    , m_isLocked(false)
{}

ImmobileLayer::~ImmobileLayer() {}
    
void ImmobileLayer::setIndex(const std::shared_ptr<IIndex>& index)
{
    m_index = index;
}

void ImmobileLayer::setOrder(const std::shared_ptr<IOrder>& order)
{
    m_order = order;
    if (m_order)
        m_order->setObjectToID(m_indexByObj);
}

void ImmobileLayer::setViewBox(const BoundingBox& viewBox)
{
    if (m_viewBox == viewBox)
        return;
    m_viewBox = viewBox;
    resetCaches();
	updateOffset(m_viewBox);
}

void ImmobileLayer::setGameBox(const boost::optional<BoundingBox>& gameBox)
{
    if (m_index) {
        m_index->setGameBox(gameBox);
        if (!m_isGameBoxInited) {
            m_isGameBoxInited = true;
            for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
                if (it->second.drawable)
                    m_index->insert(it->first, it->second.drawable);
            }
        }
    }
	m_gameBox = gameBox;
	resetCaches();
}

int ImmobileLayer::addObject(const std::shared_ptr<IObject>& obj)
{
    auto id = Identifiable::generateID(obj.get(), m_nextID);
    insertObject(id, obj);
    return id;
}

void ImmobileLayer::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    if (m_isLocked) {
        g_temp.delayedTasks.push_back([this, id, obj]() { insertObject(id, obj); });
        return;
    }

    m_needToUpdate = true;

    if (hasObject(id))
        removeObject(id);

    m_nextID = std::max(m_nextID, id + 1);

    ObjDesc desc;
    desc.obj = obj;
    if (auto positionable = dynamic_cast<IPositionable*>(obj.get()))
        positionable->setParentPosition(this);
    desc.drawable = dynamic_cast<Drawable*>(obj.get());
    desc.findable = dynamic_cast<IFindable*>(obj.get());
    if (m_registerBuilder)
        m_registerBuilder->registerObject(obj.get());
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj.get()))
        identifiable->setID(id);
    addToIndex(id, obj.get());
    
    if (desc.drawable && m_curBox.isValid()) {
        desc.drawable->setBox(m_curBox);
        desc.drawable->loadResources();
    }

    m_objects[id] = desc;
    m_indexByObj[obj.get()] = id;
}

void ImmobileLayer::insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects)
{
    for (auto it = objects.begin(); it != objects.end(); ++it)
        insertObject(it->first, it->second);
}

void ImmobileLayer::removeObject(int id)
{
    if (m_isLocked) {
		g_temp.delayedTasks.push_back([this, id]() { removeObject(id); });
        return;
    }

    m_needToUpdate = true;

    auto it = m_objects.find(id);
    if (it == m_objects.end())
        return;
    auto obj = it->second.obj.get();
    m_indexByObj.erase(obj);
    if (m_index)
        m_index->remove(id);
    if (m_db)
        m_db->remove(id);
    m_register.remove(obj);
    m_objects.erase(it);

    resetCaches();
}

IObject* ImmobileLayer::getIObject(int id) const
{
    auto it = m_objects.find(id);
    if (it == m_objects.end())
        THROW_EX() << "Can't find object with ID: " << id;
    return it->second.obj.get();
}

void ImmobileLayer::clear()
{
	if (m_isLocked) {
		g_temp.delayedTasks.push_back([this]() { clear(); });
        return;
    }

    m_needToUpdate = true;
    m_objects.clear();
    m_indexByObj.clear();
    if (m_index)
        m_index->clear();
    if (m_db)
        m_db->clear();
    m_register.clear();

    resetCaches();
}
    
std::shared_ptr<IObject> ImmobileLayer::getIObjectSPtr(int id) const
{
    auto it = m_objects.find(id);
    if (it == m_objects.end())
        THROW_EX() << "Can't find object with ID: " << id;
    return it->second.obj;
}

void ImmobileLayer::update()
{
    m_needToUpdate = true;
    updateIndexIfNeeded();
	resetCaches();
}

std::shared_ptr<IObject> ImmobileLayer::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    auto transformedPoint = position().inversed() * point;
    static std::vector<IFindable*> findables;
    findables.clear();
    if (m_index) {
        m_index->findablesByBox(BoundingBox(transformedPoint), findables);
        if (findables.empty())
            return nullptr;
        if (m_order)
            m_order->sort(findables);
    } else {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->second.findable && it->second.drawable)
                findables.push_back(it->second.findable);
        }
        if (m_order)
            m_order->sort(findables);
    }
    if (!m_order)
        std::reverse(findables.begin(), findables.end());
    for (auto it = findables.rbegin(); it != findables.rend(); ++it) {
        if (auto obj = (*it)->findChildByPoint(transformedPoint))
            return obj;
        if ((*it)->isSelectableByPoint(transformedPoint))
            return getIObjectSPtr(*it);
    }
    return nullptr;
}

void ImmobileLayer::loadResources()
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second.drawable)
            it->second.drawable->loadResources();
    }
}

void ImmobileLayer::drawAt(const Transform2& position) const
{
    calcDrawables();
    m_isLocked = true;
    for (auto it = m_cachedDrawables.begin(); it != m_cachedDrawables.end(); ++it)
        (*it)->draw(position);
    m_isLocked = false;
}

void ImmobileLayer::setBox(const BoundingBox& allowedBox)
{
    m_curBox = allowedBox;
    setPositionBoxes(allowedBox, allowedBox);
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it->second.drawable)
            it->second.drawable->setBox(allowedBox);
    }
}

void ImmobileLayer::registerObject(PropertiesRegisterBuilder* builder)
{
    m_registerBuilder.reset(new PropertiesRegisterBuilder(*builder));
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        builder->registerObject(it->second.obj.get());
}
    
void ImmobileLayer::serialize(Serializer& s) const
{
    std::map<int, std::shared_ptr<IObject>> objects;
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
        objects[it->first] = it->second.obj;
    s << "index" << m_index << "order" << m_order << "objects" << objects;
}

void deserializeLayerContents(Deserializer& deserializer, ImmobileLayer* layer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(Objects, objects);
    DESERIALIZE(std::shared_ptr<IIndex>, index);
    DESERIALIZE(std::shared_ptr<IOrder>, order);
    layer->setIndex(index);
    layer->setOrder(order);
    layer->insertObjects(objects);
}

std::unique_ptr<IObject> deserializeImmobileLayer(Deserializer& deserializer)
{
    std::unique_ptr<ImmobileLayer> result(new ImmobileLayer());
    deserializeLayerContents(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(ImmobileLayer);

const std::vector<std::shared_ptr<IObject>>& ImmobileLayer::objectsAsList() const
{
    if (m_cachedAllObjs.empty()) {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
            m_cachedAllObjs.push_back(it->second.obj);
    }
    return m_cachedAllObjs;
}

void ImmobileLayer::delayedUpdate() const
{
	m_updateHandle = Handle([this]() { setNeedUpdate(); });
}

const std::vector<Drawable*>& ImmobileLayer::drawablesInView() const
{
    calcDrawables();
    return m_cachedDrawables;
}

const std::vector<IFindable*>& ImmobileLayer::findablesByBox(const BoundingBox& box) const
{
    static std::vector<IFindable*> findables;
    findables.clear();
    if (m_index) {
        updateIndexIfNeeded();
        m_index->findablesByBox(box, findables);
        if (findables.empty())
            return findables;
    } else {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->second.findable && it->second.drawable)
                findables.push_back(it->second.findable);
        }
    }
    if (m_independent) {
        if (m_order)
            m_order->sort(findables);
        else
            std::reverse(findables.begin(), findables.end());
    }
    return findables;
}

int ImmobileLayer::indexByObj(IObject* obj) const
{
    auto it = m_indexByObj.find(obj);
    if (it == m_indexByObj.end())
        THROW_EX() << "Can't find index of object, it isn't part of this layer";
    return it->second;
}

void ImmobileLayer::addToIndex(int id, IObject* obj)
{
    if (m_index) {
        if (auto* drawable = dynamic_cast<Drawable*>(obj)) {
            if (m_isGameBoxInited) {
                m_index->insert(id, drawable);
            }
        }
        m_needToUpdate = true;
    }
    resetCaches();
}

void ImmobileLayer::resetCaches() const
{
    m_cachedDrawables.clear();
    m_cachedAllObjs.clear();
}

void ImmobileLayer::updateIndexIfNeeded() const
{
    if (m_needToUpdate) {
        m_index->update();
        m_needToUpdate = false;
    }
}

void ImmobileLayer::calcDrawables() const
{
    if (m_cachedDrawables.empty()) {
        if (m_index) {
            updateIndexIfNeeded();
            m_index->drawablesByBox(m_viewBox, m_cachedDrawables);
        } else {
            m_cachedDrawables = getObjects<Drawable>();
        }
        if (m_order && m_independent)
            m_order->sort(m_cachedDrawables);
    }
}

} }
