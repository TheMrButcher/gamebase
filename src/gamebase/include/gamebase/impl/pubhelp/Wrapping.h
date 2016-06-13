#pragma once

#include <gamebase/impl/pubhelp/FromImpl.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <boost/optional.hpp>
#include <vector>

namespace gamebase { namespace impl {

template <typename T>
boost::optional<T> tryWrap(const std::shared_ptr<IObject>& obj)
{
    auto castedImpl = FromImpl<T>::cast(obj);
    if (!castedImpl)
        return boost::none;
    return T(castedImpl);
}

template <typename T>
std::vector<T> wrap(const std::vector<std::shared_ptr<IObject>>& objects)
{
    std::vector<T> result;
    result.reserve(objects.size());

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        auto castedImpl = FromImpl<T>::cast(*it);
        if (castedImpl)
            result.push_back(T(castedImpl));
    }

    return std::move(result);
}

template <typename T>
std::shared_ptr<IObject> unwrap(const T& obj)
{
    return ToImpl<T>::cast(obj);
}

} }
