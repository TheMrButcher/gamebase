/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/TextGeometry.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/impl/graphics/GLColor.h>
#include <vector>

namespace gamebase { namespace impl {

class ITextRenderer {
public:
    virtual ~ITextRenderer() {}

    virtual void load(const std::vector<CharPosition>& textGeom) = 0;

    virtual void setColor(const GLColor& color) = 0;
    virtual void setOutlineColor(const GLColor& color) = 0;
    virtual void setUnderlined(bool enabled) = 0;
    virtual void setLineThrough(bool enabled) = 0;
    
    virtual bool empty() const = 0;

    virtual void render(const Transform2& pos) = 0;
};

} }
