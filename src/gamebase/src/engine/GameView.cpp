#include <stdafx.h>
#include <gamebase/engine/GameView.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/Application.h>
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/graphics/Clipping.h>

namespace gamebase {

GameView::GameView(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_box(box)
    , m_viewBox(Vec2(0, 0))
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}

Vec2 GameView::setViewCenter(const Vec2& v)
{
    if (m_box->isValid()) {
        m_viewBox = BoundingBox(box().width(), box().height(), v);
        if (m_gameBox.isValid()) {
            if (m_viewBox.bottomLeft.x < m_gameBox.bottomLeft.x)
                m_viewBox.move(Vec2(m_gameBox.bottomLeft.x - m_viewBox.bottomLeft.x, 0));
            if (m_viewBox.topRight.x > m_gameBox.topRight.x)
                m_viewBox.move(Vec2(m_gameBox.topRight.x - m_viewBox.topRight.x, 0));
            if (m_viewBox.bottomLeft.y < m_gameBox.bottomLeft.y)
                m_viewBox.move(Vec2(0, m_gameBox.bottomLeft.y - m_viewBox.bottomLeft.y));
            if (m_viewBox.topRight.y > m_gameBox.topRight.y)
                m_viewBox.move(Vec2(0, m_gameBox.topRight.y - m_viewBox.topRight.y));
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

void GameView::setGameBox(const BoundingBox& box)
{
    m_gameBox = box;
    const auto& objs = m_canvas->objectsAsList();
    for (auto it = objs.begin(); it != objs.end(); ++it) {
        if (auto* layer = dynamic_cast<ILayer*>(it->get()))
            layer->setGameBox(box);
    }
    setViewCenter(m_viewBox.center());
}

bool GameView::isMouseOn() const
{
    auto fullPos = fullTransform();
    auto fullBox = box().transformed(fullPos);
    return fullBox.contains(app->inputRegister().mousePosition());
}

Vec2 GameView::mouseCoords() const
{
    auto fullPos = fullTransform().offset;
    auto mousePos = app->inputRegister().mousePosition();
    mousePos -= fullPos;
    mousePos += viewCenter();
    return mousePos;
}

void GameView::insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers)
{
    std::map<int, std::shared_ptr<IObject>> objects;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        objects.insert(objects.end(),
            std::make_pair(it->first, std::shared_ptr<IObject>(it->second)));
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
    m_box->setParentBox(allowedBox);
    m_canvas->setBox(m_box->get());
    setPositionBoxes(allowedBox, box());
    setViewCenter(m_viewBox.center());
}

void GameView::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("layers", &m_canvas->objectsCollection());
}

void GameView::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "layers" << m_canvas->objectsAsMap()
        << "viewCenter" << m_viewBox.center() << "isLimited" << m_gameBox.isValid()
        << "gameBox" << (m_gameBox.isValid() ? m_gameBox : BoundingBox(Vec2(0, 0)));
}

std::unique_ptr<IObject> deserializeGameView(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<ILayer>> Layers;
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Layers, layers);
    DESERIALIZE(Vec2, viewCenter);
    DESERIALIZE(bool, isLimited);
    DESERIALIZE(BoundingBox, gameBox);
    std::unique_ptr<GameView> result(new GameView(box, position));
    result->insertLayers(layers);
    result->setViewCenter(viewCenter);
    result->setGameBox(isLimited ? gameBox : BoundingBox());
    return std::move(result);
}

REGISTER_CLASS(GameView);

}
