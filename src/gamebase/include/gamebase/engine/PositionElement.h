#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

class PositionElement : virtual public IPositionable, public Registrable, public ISerializable {
public:
    virtual Vec2 getOffset() const { THROW_EX() << typeid(this).name() << "getOffset is not implemented"; }
    virtual void setOffset(const Vec2& v) { THROW_EX() << typeid(this).name() << "setOffset is not implemented"; }

    virtual float scale() const { THROW_EX() << typeid(this).name() << "scale is not implemented"; }
    virtual void setScale(float scale) { THROW_EX() << typeid(this).name() << "setScale is not implemented"; }

    virtual float scaleX() const { THROW_EX() << typeid(this).name() << "scaleX is not implemented"; }
    virtual void setScaleX(float scale) { THROW_EX() << typeid(this).name() << "setScaleX is not implemented"; }

    virtual float scaleY() const { THROW_EX() << typeid(this).name() << "scaleY is not implemented"; }
    virtual void setScaleY(float scale) { THROW_EX() << typeid(this).name() << "setScaleY is not implemented"; }

    virtual float angle() const { THROW_EX() << typeid(this).name() << "angle is not implemented"; }
    virtual void setAngle(float angle) { THROW_EX() << typeid(this).name() << "setAngle is not implemented"; }
};

}
