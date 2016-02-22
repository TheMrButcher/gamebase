#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/graphics/GLTexture.h>
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

    virtual void setTexture(const GLTexture& texture) { m_texture = texture; }
    void setColor(const Color& color) { m_color = color; }
    const Color& color() const { return m_color; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override { m_rect = allowedBox; }
    virtual BoundingBox box() const override { return m_rect; }

    void registerProperties(const std::string& prefix, PropertiesRegisterBuilder* builder);

protected:
    BoundingBox m_rect;
    GLBuffers m_buffers;
    GLTexture m_texture;
    Color m_color;
};

}
