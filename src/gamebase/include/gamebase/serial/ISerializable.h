#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

class ISerializer;

class ISerializable : public virtual IObject {
public:
    virtual void serialize(ISerializer* s) const = 0;
};

}
