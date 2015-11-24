#pragma once

#include <gamebase/engine/PositionElement.h>
#include <gamebase/engine/RotatedPosition.h>

namespace gamebase {

class RotatedPositionElement : public RotatedPosition, public PositionElement {
public:
    RotatedPositionElement() {}

    RotatedPositionElement(float angle, const Vec2& offset = Vec2())
        : RotatedPosition(angle, offset)
    {}

    virtual Vec2 getOffset() const override { return RotatedPosition::getOffset(); }
    virtual void setOffset(const Vec2& v) override { RotatedPosition::setOffset(v); }

    virtual float angle() const override { return RotatedPosition::angle(); }
    virtual void setAngle(float angle) override { RotatedPosition::setAngle(angle); }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& serializer) const override;
};

}
