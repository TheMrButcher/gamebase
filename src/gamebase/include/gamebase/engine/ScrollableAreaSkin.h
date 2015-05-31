#pragma once

#include <gamebase/engine/ScrollBar.h>

namespace gamebase {

class ScrollableAreaSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue,
        Direction::Enum direction) const = 0;

    virtual BoundingBox areaBox() const = 0;
    virtual void setSize(float width, float height) = 0;
};

}
