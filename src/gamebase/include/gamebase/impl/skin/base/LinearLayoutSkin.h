#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/engine/Direction.h>
#include <gamebase/impl/relpos/IRelativeOffset.h>

namespace gamebase { namespace impl {

class LinearLayoutSkin : public Drawable, public Registrable {
public:
    virtual BoundingBox listBox() const = 0;
    virtual Direction::Enum direction() const = 0;
    virtual std::shared_ptr<IRelativeOffset> createOffset(size_t index) const = 0;
    virtual void setExtent(const BoundingBox& box) = 0;
    virtual void setFixedBox(float width, float height) = 0;
};

} }
