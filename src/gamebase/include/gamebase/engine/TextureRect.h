#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/graphics/Texture.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase {

class GAMEBASE_API TextureRect : public Drawable {
public:
    TextureRect(const IPositionable* position = nullptr)
        : Drawable(position)
        , m_color(1, 1, 1)
    {}

    void setTexture(const Texture& texture) { m_texture = texture; }
    void setColor(const Color& color) { m_color = color; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override { m_rect = allowedBox; }
    virtual BoundingBox box() const override { return m_rect; }

private:
    BoundingBox m_rect;
    GLBuffers m_buffers;
    Texture m_texture;
    Color m_color;
};

}
