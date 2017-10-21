/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename T>
T makeRaw(const T& obj)
{
    return impl::wrap<T>(impl::unwrapRaw<T>(obj));
}

}
