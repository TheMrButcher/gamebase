#include <stdafx.h>
#include <gamebase/engine/ImmobileLayer.h>
#include <gamebase/engine/InactiveObjectConstruct.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

ImmobileLayer::ImmobileLayer()
    : m_isGameBoxInited(false)
    , m_needToUpdate(false)
    , m_nextID(0)
{}
    
void ImmobileLayer::setIndex(const std::shared_ptr<IIndex>& index)
{
    m_index = index;
}

void ImmobileLayer::setOrder(const std::shared_ptr<IOrder>& order)
{
    m_order = order;
}

void ImmobileLayer::setViewBox(const BoundingBox& viewBox)
{
    if (m_viewBox == viewBox)
        return;
    m_viewBox = viewBox;
    resetCaches();
    setOffset(-viewBox.center());
}

void ImmobileLayer::setGameBox(const BoundingBox& gameBox)
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
}

int ImmobileLayer::addObject(const std::shared_ptr<IObject>& obj)
{
    auto id = Identifiable::generateID(obj.get(), m_nextID);
    insertObject(id, obj);
    return id;
}

void ImmobileLayer::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
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
    auto it = m_objects.find(id);
    if (it == m_objects.end())
        return;
    auto obj = it->second.obj.get();
    m_objects.erase(it);
    m_indexByObj.erase(obj);
    if (m_index)
        m_index->remove(id);
    if (m_db)
        m_db->remove(id);
    m_register.remove(obj);

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

std::shared_ptr<IObject> ImmobileLayer::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    auto transformedPoint = position().inversed() * point;
    static std::vector<IFindable*> findables;
    findables.clear();
    if (m_index) {
        auto foundObjs = m_index->findByBox(BoundingBox(transformedPoint));
        if (foundObjs.empty())
            return nullptr;
        if (m_order)
            m_order->sort(foundObjs);
        for (auto it = foundObjs.begin(); it != foundObjs.end(); ++it) {
            if (auto* findable = dynamic_cast<IFindable*>(*it))
                findables.push_back(findable);
        }
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
    for (auto it = findables.begin(); it != findables.end(); ++it) {
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
    if (!m_index && !m_order) {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->second.drawable)
                it->second.drawable->draw(position);
        }
        return;
    }
    if (m_cachedDrawables.empty()) {
        if (m_index) {
            updateIndexIfNeeded();
            m_cachedDrawables = m_index->findByBox(m_viewBox);
        } else {
            m_cachedDrawables = getObjects<Drawable>();
        }
        if (m_order)
            m_order->sort(m_cachedDrawables);
    }
    for (auto it = m_cachedDrawables.begin(); it != m_cachedDrawables.end(); ++it)
        (*it)->draw(position);
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
    s << "objects" << objects << "index" << m_index << "order" << m_order;
}

std::unique_ptr<IObject> deserializeImmobileLayer(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(Objects, objects);
    DESERIALIZE(std::shared_ptr<IIndex>, index);
    DESERIALIZE(std::shared_ptr<IOrder>, order);
    std::unique_ptr<ImmobileLayer> result(new ImmobileLayer());
    result->setIndex(index);
    result->setOrder(order);
    result->insertObjects(objects);
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

}
