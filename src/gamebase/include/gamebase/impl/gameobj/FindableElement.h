#pragma once

#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FindableElement : public virtual IFindable, public Registrable, public ISerializable {
public:
    virtual void connectWith(const IObject* obj) = 0;

    virtual void setBox(const BoundingBox& box) = 0;
};

} }
