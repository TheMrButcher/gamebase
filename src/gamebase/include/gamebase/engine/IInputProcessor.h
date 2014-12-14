#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/engine/InputRegister.h>

namespace gamebase {

class IInputProcessor : public virtual IObject {
public:
    virtual void processInput(const InputRegister& input) {}
};

}
