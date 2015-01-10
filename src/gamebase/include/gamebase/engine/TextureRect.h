#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/graphics/Texture.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class GAMEBASE_API TextureRect : public IDrawable {
public:
    TextureRect(const IPositionable* position = nullptr)
        : m_drawPosition(position)
        , m_color(1, 1, 1)
    {}

    void setTexture(const Texture& texture) { m_texture = texture; }
    void setColor(const Color& color) { m_color = color; }

    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;
    virtual void setBox(const BoundingBox& allowedBox) override { m_rect = allowedBox; }
    virtual BoundingBox box() const override { return m_rect; }

private:
    const IPositionable* m_drawPosition;
    
    BoundingBox m_rect;
    GLBuffers m_buffers;
    Texture m_texture;
    Color m_color;
};

}
