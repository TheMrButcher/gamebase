#pragma once

#include <gamebase/impl/pubhelp/FromImpl.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>
#include <gamebase/tools/Exception.h>
#include <boost/optional.hpp>
#include <vector>

namespace gamebase { namespace impl {

template <typename T>
boost::optional<T> tryWrap(const SmartPointer<IObject>& obj)
{
    auto castedImpl = FromImpl<T>::cast(obj);
    if (!castedImpl)
        return boost::none;
    return T(castedImpl);
}

template <typename T>
boost::optional<T> tryWrap(const std::shared_ptr<IObject>& obj)
{
    return tryWrap<T>(SmartPointer<IObject>(obj));
}

template <typename T>
boost::optional<T> tryWrap(IObject* obj)
{
    return tryWrap<T>(SmartPointer<IObject>(obj));
}

template <typename T, typename ParentType>
T findAndWrap(ParentType* parent, const std::string& name)
{
    auto obj = parent->getAbstractChild(name);
    auto result = tryWrap<T>(obj);
    if (!result)
        THROW_EX() << "Can't convert object with name " << name
            << " from type " << typeid(*obj).name() << " to " << typeid(T).name();
    return *result;
}

template <typename T>
T wrap(const SmartPointer<IObject>& obj)
{
    auto result = tryWrap<T>(obj);
    if (!result)
        THROW_EX() << "Can't convert from type " << typeid(*obj).name() << " to " << typeid(T).name();
    return *result;
}

template <typename T>
T wrap(const std::shared_ptr<IObject>& obj)
{
    return wrap<T>(SmartPointer<IObject>(obj));
}

template <typename T>
T wrap(IObject* obj)
{
    return wrap<T>(SmartPointer<IObject>(obj));
}

template <typename T, typename Iter>
std::vector<T> wrap(Iter begin, Iter end)
{
    std::vector<T> result;
    if (begin >= end)
        return result;
    result.reserve(static_cast<size_t>(std::distance(begin, end)));

    for (auto it = begin; it != end; ++it) {
        auto castedImpl = FromImpl<T>::cast(SmartPointer<IObject>(*it));
        if (castedImpl)
            result.push_back(T(castedImpl));
    }

    return std::move(result);
}

template <typename T>
std::vector<T> wrap(const std::vector<std::shared_ptr<IObject>>& objects)
{
    return wrap<T>(objects.begin(), objects.end());
}

template <typename T>
std::vector<T> wrap(const std::vector<IObject*>& objects)
{
    return wrap<T>(objects.begin(), objects.end());
}

template <typename T>
IObject* unwrapRaw(const T& obj)
{
    return ToImpl<T>::castRaw(obj);
}

template <typename T>
std::shared_ptr<IObject> unwrapShared(const T& obj)
{
    return ToImpl<T>::castShared(obj);
}

template <typename T>
SmartPointer<IObject> unwrapSmart(const T& obj)
{
    auto sharedImpl = unwrapShared(obj);
    if (sharedImpl)
        return SmartPointer<IObject>(sharedImpl);
    return SmartPointer<IObject>(unwrapRaw(obj));
}

} }
