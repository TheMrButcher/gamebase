/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/ObjectReflection.h>
#include <gamebase/impl/tools/TopViewLayoutSlot.h>
#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

void TopViewLayoutSlot::init(
    IPositionable* parentPositionable,
    IDrawable* parentDrawable,
    const std::shared_ptr<IObject>& object)
{
    reset();
    auto reflection = std::make_shared<ObjectReflection>(
        parentPositionable, parentDrawable);
    reflection->addObject(object);
    m_id = app->topViewLayout()->addObject(reflection);
}

void TopViewLayoutSlot::reset()
{
    if (m_id) {
        app->topViewLayout()->removeObject(*m_id);
        m_id.reset();
    }
}

} }
