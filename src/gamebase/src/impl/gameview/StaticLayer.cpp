/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/StaticLayer.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

StaticLayer::StaticLayer()
    : m_isGameBoxInited(false)
    , m_needToUpdate(false)
    , m_independent(true)
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}
    
void StaticLayer::setIndex(const std::shared_ptr<IIndex>& index)
{
    m_index = index;
    if (m_index)
        m_index->disableFindablesIndex();
}

void StaticLayer::setOrder(const std::shared_ptr<IOrder>& order)
{
    m_order = order;
}

void StaticLayer::setViewBox(const BoundingBox& viewBox)
{
    if (m_viewBox == viewBox)
        return;
    m_viewBox = viewBox;
    m_cachedDrawables.clear();
    updateOffset(m_viewBox);
}

void StaticLayer::setGameBox(const boost::optional<BoundingBox>& gameBox)
{
    if (m_index) {
        m_index->setGameBox(gameBox);
        if (!m_isGameBoxInited) {
            m_isGameBoxInited = true;
            for (auto it = m_objsToIndex.begin(); it != m_objsToIndex.end(); ++it)
                m_index->insert(it->first, it->second);
            m_objsToIndex.clear();
        }
    }
    m_gameBox = gameBox;
    m_cachedDrawables.clear();
}

int StaticLayer::addObject(const std::shared_ptr<IObject>& obj)
{
    auto id = Identifiable::generateID(obj.get(), m_canvas->nextID());
    m_canvas->insertObject(id, obj);
    addToIndex(id, obj.get());
    return id;
}

void StaticLayer::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj.get()))
        identifiable->setID(id);
    m_canvas->insertObject(id, obj);
    addToIndex(id, obj.get());
}

void StaticLayer::insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects)
{
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (auto* identifiable = dynamic_cast<Identifiable*>(it->second.get()))
            identifiable->setID(it->first);
        addToIndex(it->first, it->second.get());
    }
    m_canvas->insertObjects(objects);
}

void StaticLayer::clear()
{
    m_canvas->clear();
    if (m_index)
        m_index->clear();
    m_objsToIndex.clear();
    m_cachedDrawables.clear();
}

std::shared_ptr<IObject> StaticLayer::getIObjectSPtr(IObject* obj) const
{
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj))
        return getIObjectSPtr(identifiable->id());
    return m_canvas->getIObjectSPtr(obj);
}

void StaticLayer::update()
{
    m_cachedDrawables.clear();
    m_index->update();
}

void StaticLayer::drawAt(const Transform2& position) const
{
    if (!m_index && !m_order) {
        m_canvas->draw(position);
        return;
    }
    calcDrawables();
    for (auto it = m_cachedDrawables.begin(); it != m_cachedDrawables.end(); ++it)
        (*it)->draw(position);
}

void StaticLayer::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("objects", &m_canvas->objectsCollection());
}
    
void StaticLayer::serialize(Serializer& s) const
{
    s << "objects" << m_canvas->objectsAsMap() << "index" << m_index << "order" << m_order;
}

std::unique_ptr<IObject> deserializeStaticLayer(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(Objects, objects);
    DESERIALIZE(std::shared_ptr<IIndex>, index);
    DESERIALIZE(std::shared_ptr<IOrder>, order);
    std::unique_ptr<StaticLayer> result(new StaticLayer());
    result->setIndex(index);
    result->setOrder(order);
    result->insertObjects(objects);
    return std::move(result);
}

REGISTER_CLASS(StaticLayer);

const std::vector<Drawable*>& StaticLayer::drawablesInView() const
{
    if (!m_index) {
        static std::vector<Drawable*> result;
        result = getObjects<Drawable>();
        return result;
    }

    calcDrawables();
    return m_cachedDrawables;
}

const std::vector<IFindable*>& StaticLayer::findablesByBox(const BoundingBox& box) const
{
    static const std::vector<IFindable*> NONE;
    return NONE;
}

void StaticLayer::updateIndexIfNeeded() const
{
    if (m_needToUpdate) {
        m_index->update();
        m_needToUpdate = false;
    }
}

void StaticLayer::addToIndex(int id, IObject* obj)
{
    if (m_index) {
        if (auto* drawable = dynamic_cast<Drawable*>(obj)) {
            if (m_isGameBoxInited) {
                m_index->insert(id, drawable);
            } else {
                m_objsToIndex.push_back(std::make_pair(id, drawable));
            }
        }
        m_needToUpdate = true;
    }
    m_cachedDrawables.clear();
}

void StaticLayer::calcDrawables() const
{
    if (m_cachedDrawables.empty()) {
        if (m_index) {
            updateIndexIfNeeded();
            m_cachedDrawables = m_index->drawablesByBox(m_viewBox);
        } else {
            m_cachedDrawables = getObjects<Drawable>();
        }
        if (m_order && m_independent)
            m_order->sort(m_cachedDrawables);
    }
}

} }
