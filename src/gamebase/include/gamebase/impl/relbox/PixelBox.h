#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API PixelBox : public IRelativeBox, public ISerializable {
public:
    PixelBox(float width, float height)
    {
        m_box = BoundingBox(
            Vec2(-0.5f * width, -0.5f * height),
            Vec2(0.5f * width, 0.5f * height));
    }

    virtual BoundingBox count(const BoundingBox&) const override { return m_box; }

    virtual void serialize(Serializer& s) const override;
};

} }
