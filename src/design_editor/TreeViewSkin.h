/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/ScrollableArea.h>
#include <gamebase/impl/ui/ToggleButton.h>
#include <gamebase/impl/relbox/IRelativeBox.h>

namespace gamebase { namespace editor {

class TreeViewSkin : public impl::Drawable {
public:
    virtual impl::BoundingBox treeBox() const = 0;
    virtual std::shared_ptr<impl::ScrollableArea> createTreeArea() const = 0;
    virtual std::shared_ptr<impl::ToggleButton> createOpenButton() const = 0;
    virtual std::shared_ptr<impl::IRelativeBox> createSubtreeBox() const = 0;
    virtual std::shared_ptr<impl::IRelativeOffset> createOffset() const = 0;
};

} }
