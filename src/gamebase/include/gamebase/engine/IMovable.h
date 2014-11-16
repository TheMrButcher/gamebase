#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IMovable : public IPositionable {
public:
    virtual void move(float time) = 0;
};

}
