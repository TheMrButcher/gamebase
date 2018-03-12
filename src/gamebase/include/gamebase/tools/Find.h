/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/Layout.h>

namespace gamebase {

template <typename ObjType>
ObjType find(Layout layout, const std::string& name)
{
    auto obj = layout.getImpl()->tryGetAbstractObject(name);
    if (!obj)
        return ObjType();
    auto optObj = impl::tryWrap<ObjType>(obj);
    if (optObj)
        return *optObj;
    return ObjType();
}

}
