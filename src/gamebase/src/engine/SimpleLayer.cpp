#include <stdafx.h>
#include <gamebase/engine/SimpleLayer.h>
#include <gamebase/engine/InactiveObjectConstruct.h>
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

int SimpleLayer::addObject(const std::shared_ptr<IObject>& obj)
{
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj.get())) {
        if (identifiable->hasValidID()) {
            m_canvas->insertObject(identifiable->id(), obj);
            return identifiable->id();
        }

        int id = m_canvas->addObject(obj);
        identifiable->setID(id);
        return id;
    }
    return m_canvas->addObject(obj);
}

void SimpleLayer::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj.get()))
        identifiable->setID(id);
    m_canvas->insertObject(id, obj);
}

void SimpleLayer::insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects)
{
    m_canvas->insertObjects(objects);
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
