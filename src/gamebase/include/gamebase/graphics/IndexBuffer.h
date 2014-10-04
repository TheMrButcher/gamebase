#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/typedefs.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API IndexBuffer {
public:
    IndexBuffer()
        : m_size(0)
    {}

    IndexBuffer(const std::vector<short>& indices)
    {
        init(&indices.front(), indices.size());
    }

    IndexBuffer(const short* indices, size_t size)
    {
        init(indices, size);
    }

    GLuint id() const { return m_id; }
    size_t size() const { return m_size; }

    void bind() const;

private:
    void init(const short* indices, size_t size);

    GLuint m_id;
    size_t m_size;
};

}
