/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename ToType, typename FromType>
ToType tryCast(FromType obj)
{
    auto optObj = impl::tryWrap<ToType>(impl::unwrapSmart(obj));
    if (optObj)
        return *optObj;
    return ToType();
}

}
