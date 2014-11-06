#include <stdafx.h>
#include <gamebase/graphics/Texture.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

Texture::Texture(const Image& image)
{
    m_size = image.size;
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.size.width, image.size.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, &image.data.front());
}

void Texture::bind() const
{
    if (m_size.width == 0 || m_size.height == 0)
        THROW_EX() << "Can't bind empty Texture";
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

}
