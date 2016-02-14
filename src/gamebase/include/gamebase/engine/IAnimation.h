#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/PropertiesRegister.h>
#include <gamebase/engine/TimeState.h>

namespace gamebase {

class IAnimation : public virtual IObject {
public:
    virtual ~IAnimation() {}

    virtual void load(const PropertiesRegister& props) = 0;

    virtual void start() = 0;

    virtual Time step(Time time) = 0;

    virtual bool isFinished() const = 0;
};

}
