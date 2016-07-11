#pragma once

#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename T>
T makeRaw(const T& obj)
{
    return impl::wrap<T>(impl::unwrapRaw<T>(obj));
}

}
