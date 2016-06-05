#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/reg/PropertiesRegister.h>

namespace gamebase { namespace impl {

class IAction : virtual public IObject {
public:
    virtual void load(const PropertiesRegister& props) = 0;

    virtual void exec() = 0;
};

} }
