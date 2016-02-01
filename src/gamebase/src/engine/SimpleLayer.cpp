#include <stdafx.h>
#include <gamebase/engine/SimpleLayer.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

SimpleLayer::SimpleLayer()
{
    m_canvas = std::make_shared<CanvasLayout>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue())); 
    m_canvas->setParentPosition(this);
}

void SimpleLayer::setViewBox(const BoundingBox& viewBox)
{
    setOffset(-viewBox.center());
}

std::shared_ptr<IObject> SimpleLayer::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    return m_canvas->findChildByPoint(position().inversed() * point);
}

void SimpleLayer::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("objects", &m_canvas->objectsCollection());
}

void SimpleLayer::serialize(Serializer& s) const
{
    s << "objects" << m_canvas->objectsAsMap();
}

std::unique_ptr<IObject> deserializeSimpleLayer(Deserializer& deserializer)
{
    typedef std::map<int, std::shared_ptr<IObject>> Objects;
    DESERIALIZE(Objects, objects);
    std::unique_ptr<SimpleLayer> result(new SimpleLayer());
    result->insertObjects(objects);
    return std::move(result);
}

REGISTER_CLASS(SimpleLayer);

}
