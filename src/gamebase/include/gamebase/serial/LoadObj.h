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
