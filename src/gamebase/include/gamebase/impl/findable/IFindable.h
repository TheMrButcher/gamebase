#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/math/Transform2.h>
#include <memory>

namespace gamebase { namespace impl {

class IFindable : public virtual IObject {
public:
    virtual bool isSelectableByPoint(const Vec2& point) const = 0;

    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const = 0;
};

} }
