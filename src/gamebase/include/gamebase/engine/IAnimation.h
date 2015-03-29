#pragma once

#include <gamebase/engine/PropertiesRegister.h>

namespace gamebase {

class IAnimation {
public:
    virtual ~IAnimation() {}

    virtual void load(const PropertiesRegister& props) = 0;

    virtual void start() = 0;

    virtual void step() = 0;

    virtual bool isFinished() const = 0;
};

}
