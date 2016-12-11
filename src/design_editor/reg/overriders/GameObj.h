/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/gameobj/FindableGeometryElement.h>
#include <gamebase/impl/gameobj/ObjectConstruct.h>

namespace gamebase { namespace editor {

void serializeObjectConstruct(const impl::IObject* obj, impl::Serializer& s);

std::unique_ptr<impl::IObject> deserializeObjectConstruct(impl::Deserializer& deserializer);

} }
