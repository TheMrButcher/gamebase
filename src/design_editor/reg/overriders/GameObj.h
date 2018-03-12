/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/gameobj/FindableGeometryElement.h>
#include <gamebase/impl/gameobj/ObjectConstruct.h>

namespace gamebase { namespace editor {

void serializeInactiveObjectConstruct(const impl::IObject* obj, impl::Serializer& s);
void serializeAnimatedObjectConstruct(const impl::IObject* obj, impl::Serializer& s);
void serializeObjectConstruct(const impl::IObject* obj, impl::Serializer& s);

std::unique_ptr<impl::IObject> deserializeObjectConstruct(impl::Deserializer& deserializer);

} }
