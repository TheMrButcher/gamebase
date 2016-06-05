#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/app/InputRegister.h>

namespace gamebase { namespace impl {

class IInputProcessor : public virtual IObject {
public:
    virtual void processInput(const InputRegister& input) {}
};

} }
