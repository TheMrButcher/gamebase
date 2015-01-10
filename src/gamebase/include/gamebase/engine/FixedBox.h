#pragma once

#include <gamebase/engine/IRelativeBox.h>

namespace gamebase {

class FixedBox : public IRelativeBox {
public:
    FixedBox(const BoundingBox& box)
    {
        m_box = box;
    }

    FixedBox(float width, float height)
    {
        m_box = BoundingBox(
            Vec2(-0.5f * width, -0.5f * height),
            Vec2(0.5f * width, 0.5f * height));
    }

    virtual BoundingBox count(const BoundingBox&) const override { return m_box; }
};

}
