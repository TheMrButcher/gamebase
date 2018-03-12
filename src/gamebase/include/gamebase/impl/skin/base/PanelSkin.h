/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/Button.h>
#include <gamebase/impl/ui/ScrollDragBar.h>

namespace gamebase { namespace impl {

class PanelSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<Button> createCloseButton() const = 0;
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const = 0;

    virtual bool isLimitedByBox() const { return true; }
    virtual bool isTransparent() const { return false; }
    virtual BoundingBox panelBox() const = 0;
};

} }
