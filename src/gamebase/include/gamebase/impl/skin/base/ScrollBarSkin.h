/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/Button.h>
#include <gamebase/impl/ui/ScrollDragBar.h>
#include <gamebase/impl/engine/Direction.h>

namespace gamebase { namespace impl {

class ScrollBarSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<Button> createDecButton() const = 0;
    virtual std::shared_ptr<Button> createIncButton() const = 0;

    virtual std::shared_ptr<ScrollDragBar> createDragBar(
        const std::shared_ptr<FixedOffset>& position) const = 0;

    virtual BoundingBox dragBox() const = 0;
    virtual Direction::Enum direction() const = 0;
    virtual bool alwaysShow() const { return false; }
    virtual float step() const = 0;
};

} }
