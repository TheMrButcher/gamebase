#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

class IMovable : virtual public IObject {
public:
    virtual void move(float time) = 0;
};

} }
