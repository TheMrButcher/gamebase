/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/GameView.h>
#include "GameBoxes.h"
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/app/Application.h>
#include <gamebase/impl/geom/RectGeometry.h>
#include <gamebase/impl/geom/PointGeometry.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/graphics/Clipping.h>

namespace gamebase { namespace impl {

GameView::GameView(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_box(box)
    , m_viewBox(Vec2(0, 0))
    , m_gameBox(std::make_shared<InfiniteGameBox>())
    , m_nextID(0)
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}

Vec2 GameView::setViewCenter(const Vec2& v)
{
    if (m_parentBox.isValid()) {
        m_viewBox = BoundingBox(box().width(), box().height(), v);
        auto gameBox = m_gameBox->box();
        if (gameBox) {
            auto gbox = *gameBox;
            if (m_viewBox.bottomLeft.x < gbox.bottomLeft.x)
                m_viewBox.move(Vec2(gbox.bottomLeft.x - m_viewBox.bottomLeft.x, 0));
            if (m_viewBox.topRight.x > gbox.topRight.x)
                m_viewBox.move(Vec2(gbox.topRight.x - m_viewBox.topRight.x, 0));
            if (m_viewBox.bottomLeft.y < gbox.bottomLeft.y)
                m_viewBox.move(Vec2(0, gbox.bottomLeft.y - m_viewBox.bottomLeft.y));
            if (m_viewBox.topRight.y > gbox.topRight.y)
                m_viewBox.move(Vec2(0, gbox.topRight.y - m_viewBox.topRight.y));
        }
        const auto& objs = m_canvas->objectsAsList();
        for (auto it = objs.begin(); it != objs.end(); ++it) {
            if (auto* layer = dynamic_cast<ILayer*>(it->get()))
                layer->setViewBox(m_viewBox);
        }
    } else {
        m_viewBox = BoundingBox(v);
    }
    return m_viewBox.center();
}

BoundingBox GameView::gameBox() const
{
    if (auto gbox = m_gameBox->box())
        return *gbox;
    return BoundingBox(
        Vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()),
        Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
}

void GameView::setGameBox(const BoundingBox& box)
{
    setGameBox(std::make_shared<FixedGameBox>(box));
}

void GameView::setGameBox(const std::shared_ptr<IRelativeBox>& box)
{
    setGameBox(std::make_shared<RelativeGameBox>(RelativeGameBox::Center, box));
}

void GameView::setGameBox(const std::shared_ptr<IGameBox>& box)
{
    m_gameBox = box;
    if (m_parentBox.isValid())
        initGameBox();
}

bool GameView::isMouseOn() const
{
    return gamebase::impl::isMouseOn(this);
}

Vec2 GameView::mouseCoords() const
{
    return gamebase::impl::mouseCoords(this) + viewCenter();
}

void GameView::addLayers(const std::vector<std::shared_ptr<ILayer>>& layers)
{
    int id = m_nextID;
    std::vector<std::shared_ptr<IObject>> objects(layers.begin(), layers.end());
    m_canvas->addObjects(objects);
    for (const auto& layer : layers)
        registerLayer(id++, layer.get());
}

void GameView::insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers)
{
    std::map<int, std::shared_ptr<IObject>> objects;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        objects.insert(objects.end(),
            std::make_pair(it->first, std::shared_ptr<IObject>(it->second)));
        registerLayer(it->first, it->second.get());
    }
    m_canvas->insertObjects(objects);
}

std::shared_ptr<IObject> GameView::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    PointGeometry pointGeom(point);
    RectGeometry rectGeom(box());
    if (!rectGeom.intersects(&pointGeom, position(), Transform2()))
        return nullptr;
    return m_canvas->findChildByPoint(position().inversed() * point);
}

void GameView::drawAt(const Transform2& position) const
{
    pushClipBox(position, box());
    m_canvas->draw(position);
    popClipBox();
}

void GameView::setBox(const BoundingBox& allowedBox)
{
    m_parentBox = allowedBox;
    m_box->setParentBox(allowedBox);
    m_canvas->setBox(m_box->get());
    setPositionBoxes(allowedBox, box());
    initGameBox();
}

void GameView::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_canvas->objectsCollection());
}

void GameView::serialize(Serializer& s) const
{
    s   << "box" << m_box << "position" << m_offset << "list" << m_canvas->objectsAsList();
}

std::unique_ptr<IObject> deserializeGameView(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    std::unique_ptr<GameView> result(new GameView(box, position));

    if (deserializer.hasMember("layers")) {
        typedef std::map<int, std::shared_ptr<ILayer>> Layers;
        DESERIALIZE(Layers, layers);
        result->insertLayers(layers);
    } else {
        DESERIALIZE(std::vector<std::shared_ptr<ILayer>>, list);
        result->addLayers(list);
    }
    return std::move(result);
}

REGISTER_CLASS(GameView);

void GameView::initGameBox()
{
    m_gameBox->setViewBox(m_box->get());
    auto gbox = m_gameBox->box();
    const auto& objs = m_canvas->objectsAsList();
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        if (auto* layer = dynamic_cast<ILayer*>(it->get()))
            layer->setGameBox(gbox);
    }
    setViewCenter(m_viewBox.center());
}

} }
