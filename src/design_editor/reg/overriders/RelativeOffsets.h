/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relpos/FixedOffset.h>

namespace gamebase { namespace editor {

void serializeFixedOffsetAsAligning(const impl::IObject* obj, impl::Serializer& s);

std::unique_ptr<impl::IObject> deserializeOffset(impl::Deserializer& deserializer);

} }
