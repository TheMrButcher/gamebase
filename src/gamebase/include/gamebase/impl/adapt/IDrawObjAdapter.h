#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class IDrawObjAdapter {
public:
    virtual ~IDrawObjAdapter() {}

    virtual bool isVisible() const = 0;
    virtual void setVisible(bool value) = 0;

    virtual Vec2 getOffset() const = 0;
    virtual void setOffset(const Vec2& v) = 0;
    virtual BoundingBox box() const = 0;
    virtual BoundingBox movedBox() const = 0;

    virtual SmartPointer<IObject> getInternalObj() const = 0;
};

} }
