#pragma once

#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/utils/Exception.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API DrawablePolyline : public Drawable {
public:
    DrawablePolyline(const IPositionable* position = nullptr)
        : Drawable(position)
    {}

    void setPolyline(const std::vector<Vec2>& polyline) { m_polyline = polyline; }
    void setWidth(float width) { m_width = width; }
    void setColor(const Color& color) { m_color = color; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& globalPosition) const override;
    virtual void setBox(const BoundingBox&) override {}
    virtual BoundingBox box() const override { THROW_EX() << "DrawablePolyline::box(): Not implemented"; }

private:
    GLBuffers m_buffers;

    std::vector<Vec2> m_polyline;
    float m_width;
    Color m_color;
};

}
