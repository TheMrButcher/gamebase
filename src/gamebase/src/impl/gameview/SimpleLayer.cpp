/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameview/SimpleLayer.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

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
    auto id = Identifiable::generateID(obj.get(), m_canvas->nextID());
    m_canvas->insertObject(id, obj);
    return id;
}

void SimpleLayer::insertObject(int id, const std::shared_ptr<IObject>& obj)
{
    if (auto* identifiable = dynamic_cast<Identifiable*>(obj.get()))
        identifiable->setID(id);
    m_canvas->insertObject(id, obj);
}

void SimpleLayer::insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects)
{
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (auto* identifiable = dynamic_cast<Identifiable*>(it->second.get()))
            identifiable->setID(it->first);
    }
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
    builder->registerObject(&m_canvas->objectsCollection());
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

const std::vector<Drawable*>& SimpleLayer::drawablesInView() const
{
    static std::vector<Drawable*> result;
    result = getObjects<Drawable>();
    return result;
}

const std::vector<IFindable*>& SimpleLayer::findablesByBox(const BoundingBox& box) const
{
    static std::vector<IFindable*> result;
    result = getObjects<IFindable>();
    return result;
}

} }
