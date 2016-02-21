#include <stdafx.h>
#include <gamebase/engine/GroupLayer.h>
#include <gamebase/engine/GameView.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

GroupLayer::GroupLayer()
    : m_gameView(nullptr)
    , m_inited(false)
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}

void GroupLayer::setOrder(const std::shared_ptr<IOrder>& order)
{
    m_order = order;
}

int GroupLayer::addLayer(const std::shared_ptr<ILayer>& layer)
{
    if (!m_inited)
        THROW_EX() << "Can't add layer, GroupLayer is not already loaded";
    int id = m_gameView->nextID();
    insertLayer(id, layer);
    return id;
}

void GroupLayer::insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers)
{
    std::map<int, std::shared_ptr<IObject>> objects;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        objects.insert(objects.end(),
            std::make_pair(it->first, std::shared_ptr<IObject>(it->second)));
        registerLayer(it->first, it->second.get());
    }
    m_canvas->insertObjects(objects);
}

void GroupLayer::setViewBox(const BoundingBox& viewBox)
{
    if (m_viewBox == viewBox)
        return;
    m_viewBox = viewBox;
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->setViewBox(viewBox);
}

void GroupLayer::setGameBox(const BoundingBox& gameBox)
{
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
        it->second->setGameBox(gameBox);
}

std::shared_ptr<IObject> GroupLayer::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    auto transformedPoint = position().inversed() * point;
    if (!m_order)
        return m_canvas->findChildByPoint(transformedPoint);

    BoundingBox searchBox(transformedPoint);
    m_cachedFindables.clear();
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it) {
        const auto& findables = it->second->findablesByBox(searchBox);
        m_cachedFindables.insert(m_cachedFindables.end(),
            findables.begin(), findables.end());
    }
    m_order->sort(m_cachedFindables);
    for (auto it = m_cachedFindables.begin(); it != m_cachedFindables.end(); ++it) {
        if (auto obj = (*it)->findChildByPoint(transformedPoint))
            return obj;
        if ((*it)->isSelectableByPoint(transformedPoint))
            return getIObjectSPtr(*it);
    }
    return nullptr;
}

void GroupLayer::loadResources()
{
    m_canvas->loadResources();
}

void GroupLayer::drawAt(const Transform2& position) const
{
    if (!m_order) {
        m_canvas->draw(position);
        return;
    }

    m_cachedDrawables.clear();
    for (auto it = m_layers.begin(); it != m_layers.end(); ++it) {
        const auto& drawables = it->second->drawablesInView();
        m_cachedDrawables.insert(m_cachedDrawables.end(),
            drawables.begin(), drawables.end());
    }
    m_order->sort(m_cachedDrawables);
    for (auto it = m_cachedDrawables.begin(); it != m_cachedDrawables.end(); ++it)
        (*it)->draw(position);
}

void GroupLayer::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("layers", &m_canvas->objectsCollection());
    if (!m_inited) {
        m_gameView = m_register.findParentOfType<GameView>();
        m_inited = true;
        for (auto it = m_layers.begin(); it != m_layers.end(); ++it)
            m_gameView->registerLayer(it->first, it->second);
    }
}
    
void GroupLayer::serialize(Serializer& s) const
{
    s << "layers" << m_canvas->objectsAsMap() << "order" << m_order;
}

std::unique_ptr<IObject> deserializeGroupLayer(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<ILayer>> Layers;
    DESERIALIZE(Layers, layers);
    DESERIALIZE(std::shared_ptr<IOrder>, order);
    std::unique_ptr<GroupLayer> result(new GroupLayer());
    result->setOrder(order);
    result->insertLayers(layers);
    return std::move(result);
}

REGISTER_CLASS(GroupLayer);

void GroupLayer::registerLayer(int id, ILayer* layer)
{
    m_layers[id] = layer;
    if (m_inited)
        m_gameView->registerLayer(id, layer);
}

void GroupLayer::removeFromRegister(int id)
{
    m_layers.erase(id);
    if (m_inited)
        m_gameView->removeFromRegister(id);
}

}
