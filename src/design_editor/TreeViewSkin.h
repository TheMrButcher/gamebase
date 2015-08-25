#pragma once

#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/engine/ToggleButton.h>
#include <gamebase/engine/IRelativeBox.h>

namespace gamebase { namespace editor {

class TreeViewSkin : public Drawable {
public:
    virtual BoundingBox treeBox() const = 0;
    virtual std::shared_ptr<ScrollableArea> createTreeArea() const = 0;
    virtual std::shared_ptr<ToggleButton> createOpenButton() const = 0;
    virtual std::shared_ptr<IRelativeBox> createSubtreeBox() const = 0;
    virtual std::shared_ptr<IRelativeOffset> createOffset() const = 0;
};

} }
