/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/Deserialize.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename T>
T loadObj(const std::string& fname)
{
    auto objImpl = impl::deserializeObj(fname);
    return impl::wrap<T>(objImpl);
}

}
