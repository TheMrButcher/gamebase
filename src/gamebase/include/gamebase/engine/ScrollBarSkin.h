#pragma once

#include <gamebase/engine/Button.h>
#include <gamebase/engine/ScrollDragBar.h>
#include <gamebase/engine/Direction.h>

namespace gamebase {

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

}
