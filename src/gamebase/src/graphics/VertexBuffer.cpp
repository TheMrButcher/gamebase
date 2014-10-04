#include <stdafx.h>
#include <gamebase/graphics/VertexBuffer.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

void VertexBuffer::bind() const
{
    if (m_size == 0)
        THROW_EX() << "Can't bind empty VertexBuffer";
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::init(const float* vertices, size_t size)
{
    m_size = size;
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);
}

}
