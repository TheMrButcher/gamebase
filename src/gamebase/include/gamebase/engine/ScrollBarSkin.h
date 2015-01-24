#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/ScrollDragBar.h>

namespace gamebase {

class ScrollBarSkin : public IDrawable {
public:
    virtual std::shared_ptr<Button> createDecButton() const = 0;
    virtual std::shared_ptr<Button> createIncButton() const = 0;

    virtual std::shared_ptr<ScrollDragBar> createDragBar(
        const std::shared_ptr<FixedOffset>& position,
        const std::shared_ptr<FloatValue>& controlledValue) const = 0;

    virtual BoundingBox dragBox() const = 0;
    virtual Direction::Enum direction() const = 0;
};

}
