#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/typedefs.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API VertexBuffer {
public:
    VertexBuffer()
        : m_size(0)
    {}

    VertexBuffer(const std::vector<float>& vertices)
    {
        init(&vertices.front(), vertices.size());
    }

    VertexBuffer(const float* vertices, size_t size)
    {
        init(vertices, size);
    }

    GLuint id() const { return m_id; }
    size_t size() const { return m_size; }

    void bind() const;

private:
    void init(const float* vertices, size_t size);

    GLuint m_id;
    size_t m_size;
};

}
