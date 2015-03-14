#pragma once

#include <gamebase/engine/Button.h>
#include <gamebase/engine/ScrollDragBar.h>

namespace gamebase {

class PanelSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<Button> createCloseButton() const = 0;
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const = 0;

    virtual bool isLimitedByBox() const { return true; }
    virtual BoundingBox panelBox() const = 0;
};

}
