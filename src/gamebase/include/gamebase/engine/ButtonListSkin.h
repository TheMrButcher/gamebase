#pragma once

#include <gamebase/engine/ScrollBar.h>

namespace gamebase {

class ButtonListSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue) const = 0;

    virtual BoundingBox listBox() const = 0;
    virtual Direction::Enum direction() const = 0;
    virtual std::shared_ptr<IRelativeOffset> createButtonOffset(size_t index) const = 0;
    virtual void setSize(float size) = 0;
};

}
