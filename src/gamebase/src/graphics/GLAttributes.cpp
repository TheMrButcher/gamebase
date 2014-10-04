#include <stdafx.h>
#include <gamebase/graphics/GLAttributes.h>

namespace gamebase {

void GLAttributes::activate() const
{
    for (auto it = m_attrs.begin(); it != m_attrs.end(); ++it) {
        glEnableVertexAttribArray(it->id);
        glVertexAttribPointer(it->id, it->size, GL_FLOAT, GL_FALSE,
            sizeof(float) * m_size,
            reinterpret_cast<void*>(sizeof(float) * it->offset));
    }

}

void GLAttributes::disable() const
{
    for (auto it = m_attrs.begin(); it != m_attrs.end(); ++it)
        glDisableVertexAttribArray(it->id);
}

}
