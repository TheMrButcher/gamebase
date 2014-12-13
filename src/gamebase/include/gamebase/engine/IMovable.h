#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IMovable : public IPositionable {
public:
    IMovable(const IPositionable* parent = nullptr)
        : IPositionable(parent)
    {}

    virtual void move(float time) = 0;
};

}
