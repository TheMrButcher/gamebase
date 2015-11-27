#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

class IMovable : virtual public IObject {
public:
    virtual void move(float time) = 0;
};

}
