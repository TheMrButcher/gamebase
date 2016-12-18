/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <reg/ClassRegistrar.h>
#include <gamebase/impl/ui/LinearLayout.h>

namespace gamebase { namespace editor {

void serializeLinearLayout(const impl::IObject* obj, impl::Serializer& s);
std::unique_ptr<impl::IObject> deserializeLinearLayout(impl::Deserializer& deserializer);

} }
