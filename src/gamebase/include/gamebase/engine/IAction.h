#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/PropertiesRegister.h>

namespace gamebase {

class IAction : virtual public IObject {
public:
    virtual void load(const PropertiesRegister& props) = 0;

    virtual void exec() = 0;
};

}
