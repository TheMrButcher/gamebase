#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class IPositionable : public virtual IObject {
public:
    virtual Transform2 position() const = 0;

    virtual Transform2 transform() const = 0;
};

}
