/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/FontDesc.h>
#include <gamebase/impl/text/FontStorage.h>

namespace gamebase { namespace impl {

std::shared_ptr<IFont> FontDesc::get() const
{
    auto& storage = fontStorage();
    auto type = storage.typeOf(fontFamily);
    if (type && type == FontStorage::FontFamily::BFF)
        return storage.getFont(fontFamily, size);
    return storage.getFont(fontFamily, size, bold, italic, outlineWidth);
}

} }
