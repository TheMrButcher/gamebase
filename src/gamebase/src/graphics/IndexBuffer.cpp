#include <stdafx.h>
#include <gamebase/graphics/IndexBuffer.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

void IndexBuffer::bind() const
{
    if (m_size == 0)
        THROW_EX() << "Can't bind empty IndexBuffer";
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::init(const short* indices, size_t size)
{
    m_size = size;
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(short), indices, GL_STATIC_DRAW);
}

}
