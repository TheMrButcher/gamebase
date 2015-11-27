#pragma once

#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class GAMEBASE_API FindableElement : public virtual IFindable, public Registrable, public ISerializable {
public:
    virtual void connectWith(const IObject* obj) = 0;

    virtual void setBox(const BoundingBox& box) = 0;
};

}
