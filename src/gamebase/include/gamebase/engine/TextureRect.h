#pragma once

#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/graphics/Texture.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class TextureRect : public IDrawable {
public:
    TextureRect(const IPositionable* position)
        : m_drawPosition(position)
        , m_color(1, 1, 1)
    {}

    void setRect(const BoundingBox& rect) { m_rect = rect; }
    void setTexture(const Texture& texture) { m_texture = texture; }
    void setColor(const Color& color) { m_color = color; }

    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;

private:
    const IPositionable* m_drawPosition;
    
    BoundingBox m_rect;
    GLBuffers m_buffers;
    Texture m_texture;
    Color m_color;
};

}
