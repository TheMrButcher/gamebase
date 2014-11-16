#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IDrawable : public virtual IObject {
public:
    virtual void loadResources() = 0;

    virtual void draw(const Transform2& globalPosition) const = 0;
};

}
