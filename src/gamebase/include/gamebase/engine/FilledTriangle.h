#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>

namespace gamebase {

class GAMEBASE_API FilledTriangle : public IDrawable {
public:
    FilledTriangle(const IPositionable* position)
        : m_drawPosition(position)
        , m_color(1, 1, 1)
    {}

    void setColor(const Color& color) { m_color = color; }
    void setPoints(const std::vector<Vec2>& points) { setPoints(&points.front()); }
    void setPoints(const Vec2* points);

    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;
    virtual void setBox(const BoundingBox&) override {}
    virtual BoundingBox box() const override
    {
        BoundingBox result;
        result.enlarge(m_points[0]);
        result.enlarge(m_points[1]);
        result.enlarge(m_points[2]);
        return result;
    }

private:
    const IPositionable* m_drawPosition;
    Color m_color;
    Vec2 m_points[3];
    GLBuffers m_buffers;
};

}
