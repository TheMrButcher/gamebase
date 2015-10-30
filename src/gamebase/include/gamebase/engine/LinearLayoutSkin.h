#pragma once

#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/Direction.h>
#include <gamebase/engine/IRelativeOffset.h>

namespace gamebase {

class LinearLayoutSkin : public Drawable, public Registrable {
public:
    virtual BoundingBox listBox() const = 0;
    virtual Direction::Enum direction() const = 0;
    virtual std::shared_ptr<IRelativeOffset> createOffset(size_t index) const = 0;
    virtual void setExtent(const BoundingBox& box) = 0;
};

}
