#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/pos/TransformedPosition.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API PositionElement : public TransformedPosition, public Registrable, public ISerializable {
public:
    PositionElement() {}

    PositionElement(float scaleX, float scaleY, float angle, const Vec2& offset = Vec2())
        : TransformedPosition(scaleX, scaleY, angle, offset)
    {}

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& serializer) const override;
};

} }
