#pragma once

#include <gamebase/engine/PositionElement.h>
#include <gamebase/engine/TransformedPosition.h>

namespace gamebase {

class TransformedPositionElement : public TransformedPosition, public PositionElement {
public:
    TransformedPositionElement() {}

    TransformedPositionElement(float scaleX, float scaleY, float angle, const Vec2& offset = Vec2())
        : TransformedPosition(scaleX, scaleY, angle, offset)
    {}

    virtual Vec2 getOffset() const override { return TransformedPosition::getOffset(); }
    virtual void setOffset(const Vec2& v) override { TransformedPosition::setOffset(v); }

    virtual float scale() const override { return TransformedPosition::scale(); }
    virtual void setScale(float scale) override { TransformedPosition::setScale(scale); }

    virtual float scaleX() const override { return TransformedPosition::scaleX(); }
    virtual void setScaleX(float scale) override { TransformedPosition::setScaleX(scale); }

    virtual float scaleY() const override { return TransformedPosition::scaleY(); }
    virtual void setScaleY(float scale) override { TransformedPosition::setScaleY(scale); }

    virtual float angle() const override { return TransformedPosition::angle(); }
    virtual void setAngle(float angle) override { TransformedPosition::setAngle(angle); }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& serializer) const override;
};

}
