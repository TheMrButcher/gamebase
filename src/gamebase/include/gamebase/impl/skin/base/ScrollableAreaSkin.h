/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/ScrollBar.h>

namespace gamebase { namespace impl {

class ScrollableAreaSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue,
        Direction::Enum direction) const = 0;

    virtual BoundingBox areaBox() const = 0;
    virtual void setSize(float width, float height) = 0;
    virtual void setFixedBox(float width, float height) = 0;
};

} }
