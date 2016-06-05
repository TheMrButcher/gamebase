#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/reg/PropertiesRegister.h>
#include <gamebase/common/Time.h>

namespace gamebase { namespace impl {

class IAnimation : public virtual IObject {
public:
    virtual ~IAnimation() {}

    virtual void load(const PropertiesRegister& props) = 0;

    virtual void start() = 0;

    virtual Time step(Time time) = 0;

    virtual bool isFinished() const = 0;
};

} }
