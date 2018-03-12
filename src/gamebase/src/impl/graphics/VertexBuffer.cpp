/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/VertexBuffer.h>
#include <gamebase/tools/Exception.h>
#include <functional>

namespace gamebase { namespace impl {

void VertexBuffer::bind() const
{
    if (m_size == 0 || !m_id)
        THROW_EX() << "Can't bind empty VertexBuffer";
    glBindBuffer(GL_ARRAY_BUFFER, *m_id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::init(const float* vertices, size_t size)
{
    m_size = size;
    auto* id = new GLuint(0);
    m_id.reset(id, [](auto* id) { glDeleteBuffers(1, id); });
    glGenBuffers(1, m_id.get());
    glBindBuffer(GL_ARRAY_BUFFER, *m_id);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} }
