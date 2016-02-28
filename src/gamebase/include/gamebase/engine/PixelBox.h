#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

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

}
