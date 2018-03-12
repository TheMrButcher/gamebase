/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/geom/IdenticGeometry.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

std::unique_ptr<IObject> deserializeIdenticGeometry(Deserializer&)
{
    return std::unique_ptr<IObject>(new IdenticGeometry());
}

REGISTER_CLASS(IdenticGeometry);

} }
