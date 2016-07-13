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
