/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

InactiveObjectConstruct::InactiveObjectConstruct(
    const std::shared_ptr<IDrawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : Drawable(this)
    , m_drawable(drawable)
    , m_posElement(position ? position : std::make_shared<PositionElement>())
{}

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
    DESERIALIZE(std::shared_ptr<IDrawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    DESERIALIZE(int, id);
    std::unique_ptr<InactiveObjectConstruct> result(new InactiveObjectConstruct(skin, position));
    result->setID(id);
    return std::move(result);
}

REGISTER_CLASS(InactiveObjectConstruct);

} }
