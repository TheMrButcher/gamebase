/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

bool isMouseOn(Drawable* drawable)
{
    auto fullPos = drawable->drawPosition()->fullTransform();
    auto fullBox = drawable->box();
    fullBox.transform(fullPos);
    return fullBox.contains(app->input().mousePosition());
}

} }
