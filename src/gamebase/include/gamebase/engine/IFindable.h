#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>
#include <memory>

namespace gamebase {

class IFindable : public virtual IObject {
public:
    virtual bool isSelectableByPoint(const Vec2& point) const = 0;

    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const = 0;
};

}
