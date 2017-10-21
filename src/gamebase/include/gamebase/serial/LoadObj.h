/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/JsonDeserializer.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename T>
T loadObj(const std::string& fname)
{
    auto objImpl = impl::deserialize<impl::IObject>(fname);
    return impl::wrap<T>(objImpl);
}

}
