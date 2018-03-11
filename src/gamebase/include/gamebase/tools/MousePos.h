/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pubhelp/Wrapping.h>

namespace gamebase {

template <typename ObjType>
Vec2 mousePos(const ObjType& obj)
{
    impl::IObject* impl = impl::unwrapRaw(obj);
    return impl::mouseCoords(dynamic_cast<impl::Drawable*>(impl));
}

}
