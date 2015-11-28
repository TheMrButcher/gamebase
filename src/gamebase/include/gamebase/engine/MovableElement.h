#pragma once

#include <gamebase/engine/PositionElement.h>

namespace gamebase {

class MovableElement : virtual public IObject {
public:
    virtual void move(PositionElement* pos, float time) = 0;
};

}
