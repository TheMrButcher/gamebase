/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/graphics/GLTexture.h>
#include <gamebase/impl/graphics/GLBuffers.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/geom/BoundingBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TextureRect : public Drawable {
public:
    TextureRect(const IPositionable* position = nullptr)
        : Drawable(position)
        , m_color(1, 1, 1)
    {}

    bool isTextureLoaded() const { return m_texture.id() != 0; }
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

} }
