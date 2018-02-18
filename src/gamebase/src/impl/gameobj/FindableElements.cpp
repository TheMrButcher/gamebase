/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/FindableGeometryElement.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

FindableGeometryElement::FindableGeometryElement(
    const std::shared_ptr<IRelativeGeometry>& geom)
    : FindableGeometry(nullptr, geom)
    , m_lastValue(false)
{}

void FindableGeometryElement::serialize(Serializer& s) const
{
    s << "geom" << m_findableGeom;
}

std::unique_ptr<IObject> deserializeFindableGeometryElement(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geom);
    return std::unique_ptr<FindableGeometryElement>(new FindableGeometryElement(geom));
}

REGISTER_CLASS(FindableGeometryElement);

} }
