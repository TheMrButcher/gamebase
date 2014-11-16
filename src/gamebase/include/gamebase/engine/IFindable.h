#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IFindable : public virtual IObject {
public:
    virtual IFindable* find(
        const Vec2& point, const Transform2& globalPosition) = 0;
};

}
