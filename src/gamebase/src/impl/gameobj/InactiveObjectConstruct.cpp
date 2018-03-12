/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/gameview/GameView.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

InactiveObjectConstruct::InactiveObjectConstruct(
    const std::shared_ptr<Drawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : Drawable(this)
    , m_drawable(drawable)
    , m_posElement(position ? position : std::make_shared<PositionElement>())
{}

void InactiveObjectConstruct::kill()
{
    auto* layer = m_register.findParentOfType<ILayer>();
    layer->removeObject(this);
}

void InactiveObjectConstruct::moveToLayer(const std::string & dstLayerName)
{
    auto* gameView = properties().findParentOfType<GameView>();
    auto* dstLayer = gameView->getLayer(dstLayerName);
    moveToLayer(dstLayer);
}

void InactiveObjectConstruct::moveToLayer(ILayer* dstLayer)
{
    auto* srcLayer = properties().findParentOfType<ILayer>();
    if (srcLayer == dstLayer)
        return;
    auto obj = srcLayer->getIObjectSPtr(this);
    if (srcLayer->hasObject(obj)) {
        srcLayer->removeObject(obj);
        dstLayer->addObject(obj);
    }
}

void InactiveObjectConstruct::setFixedBox(float width, float height)
{
    if (auto resizable = dynamic_cast<IResizable*>(m_drawable.get())) {
        resizable->setFixedBox(width, height);
    } else {
        THROW_EX() << "Can't resize object, skin of type " << typeid(*m_drawable).name() << " is not resizable";
    }
}

void InactiveObjectConstruct::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_drawable.get());
    builder->registerObject("pos", m_posElement.get());
}

void InactiveObjectConstruct::serialize(Serializer& s) const
{
    s << "skin" << m_drawable << "position" << m_posElement << "id" << id();
}

std::unique_ptr<IObject> deserializeInactiveObjectConstruct(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    DESERIALIZE(int, id);
    std::unique_ptr<InactiveObjectConstruct> result(new InactiveObjectConstruct(skin, position));
    result->setID(id);
    return std::move(result);
}

REGISTER_CLASS(InactiveObjectConstruct);

} }
