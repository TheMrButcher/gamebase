/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

bool isMouseOn(const InputRegister& input, const Drawable* drawable)
{
    auto fullPos = drawable->drawPosition()->fullTransform();
    auto fullBox = drawable->box();
    fullBox.transform(fullPos);
    return fullBox.contains(input.mousePosition());
}

Vec2 mouseCoords(const InputRegister& input, const Drawable* drawable)
{
    auto fullPos = drawable->drawPosition()->fullTransform().offset;
    auto mousePos = input.mousePosition();
    mousePos -= fullPos;
    return mousePos;
}

bool isMouseOn(const Drawable* drawable)
{
    return isMouseOn(app->input(), drawable);
}

Vec2 mouseCoords(const Drawable* drawable)
{
    return mouseCoords(app->input(), drawable);
}

} }
