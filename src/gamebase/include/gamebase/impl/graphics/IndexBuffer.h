/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/graphics/typedefs.h>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

class GAMEBASE_API IndexBuffer {
public:
    IndexBuffer()
        : m_size(0)
    {}

    IndexBuffer(const std::vector<short>& indices)
        : IndexBuffer()
    {
        if (!indices.empty())
            init(&indices.front(), indices.size());
    }

    IndexBuffer(const short* indices, size_t size)
        : IndexBuffer()
    {
        if (size > 0)
            init(indices, size);
    }

    GLuint id() const { return *m_id; }
    size_t size() const { return m_size; }

    void bind() const;
    void unbind() const;

private:
    void init(const short* indices, size_t size);

    std::shared_ptr<GLuint> m_id;
    size_t m_size;
};

} }
