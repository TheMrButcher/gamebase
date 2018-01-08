/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/graphics/typedefs.h>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

class GAMEBASE_API VertexBuffer {
public:
    VertexBuffer()
        : m_size(0)
    {}

    VertexBuffer(const std::vector<float>& vertices)
        : VertexBuffer()
    {
        if (!vertices.empty())
            init(&vertices.front(), vertices.size());
    }

    VertexBuffer(const float* vertices, size_t size)
        : VertexBuffer()
    {
        if (size > 0)
            init(vertices, size);
    }

    GLuint id() const { return *m_id; }
    size_t size() const { return m_size; }

    void bind() const;
    void unbind() const;

private:
    void init(const float* vertices, size_t size);

    std::shared_ptr<GLuint> m_id;
    size_t m_size;
};

} }
