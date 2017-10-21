/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/ObjectReflection.h>
#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

std::shared_ptr<IObject> ObjectReflection::findChildByPoint(const Vec2&) const
{
    if (!isVisible())
        return nullptr;
    return m_children.findChildByPoint(
        position().inversed() * app->input().mousePosition());
}

} }
