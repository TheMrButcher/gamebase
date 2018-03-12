/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/tools/Preload.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase {

void preload(const std::string& path)
{
    auto obj = impl::deserialize<impl::IObject>(path);
    if (auto drawable = dynamic_cast<impl::IDrawable*>(obj.get())) {
        drawable->setBox(impl::BoundingBox(256, 256));
        drawable->loadResources();
    }
}

}
