#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class IDrawable : public virtual IObject {
public:
    virtual void loadResources() = 0;

    virtual void draw(const Transform2& globalPosition) const { drawAt(globalPosition); }

    virtual void drawAt(const Transform2& position) const = 0;

    virtual void setBox(const BoundingBox& allowedBox) = 0;

    virtual BoundingBox box() const = 0;

    virtual void setVisible(bool visible) = 0;

    virtual bool isVisible() const = 0;
};

}
