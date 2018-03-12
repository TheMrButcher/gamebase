/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/FilledTriangleDrawable.h>
#include <gamebase/impl/graphics/OneColorProgram.h>
namespace gamebase { namespace impl {

void FilledTriangleDrawable::setPoints(const Vec2& p0, const Vec2& p1, const Vec2& p2)
{
    m_points[0] = p0;
    m_points[1] = p1;
    m_points[2] = p2;
}

void FilledTriangleDrawable::loadResources()
{
    std::array<Vec2, 3> scaledPoints;
    auto offset = m_rect.center();
    auto size = 0.5f * m_rect.size();
    for (int i = 0; i < 3; ++i)
        scaledPoints[i] = m_points[i] * size + offset;
    m_buffers = createTriangleBuffers(scaledPoints.data());
}

void FilledTriangleDrawable::drawAt(const Transform2& position) const
{
    if (m_color.a == 0)
        return;
    const OneColorProgram& program = oneColorProgram();
    program.transform = position;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void FilledTriangleDrawable::registerProperties(
    const std::string& prefix, PropertiesRegisterBuilder* builder)
{
    if (prefix.empty()) {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    } else {
        builder->registerColor(prefix + "GLColor", &m_color);
    }
}

} }
