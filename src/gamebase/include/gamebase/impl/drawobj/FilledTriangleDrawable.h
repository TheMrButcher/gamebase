/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/graphics/GLBuffers.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/geom/BoundingBox.h>
#include <array>

namespace gamebase { namespace impl {

class GAMEBASE_API FilledTriangleDrawable : public Drawable {
public:
    FilledTriangleDrawable(const IPositionable* position = nullptr)
        : Drawable(position)
        , m_color(1, 1, 1)
    {}

    void setPoints(const Vec2& p0, const Vec2& p1, const Vec2& p2);

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
    Color m_color;
    std::array<Vec2, 3> m_points;
};

} }
