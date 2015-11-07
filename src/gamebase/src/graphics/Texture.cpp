#include <stdafx.h>
#include <gamebase/graphics/Texture.h>
#include <gamebase/utils/Exception.h>
#include "src/core/GlobalCache.h"
#include <functional>

namespace gamebase {

Texture::Texture(const Image& image)
{
    m_size = image.size;
    auto* id = new GLuint(0);
    m_id.reset(id, std::bind(glDeleteTextures, 1, id));
    glGenTextures(1, m_id.get());
    glBindTexture(GL_TEXTURE_2D, *m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.size.width, image.size.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, &image.data.front());
}

void Texture::bind() const
{
    if (m_size.width == 0 || m_size.height == 0 || !m_id)
        THROW_EX() << "Can't bind empty Texture";
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *m_id);
}

Texture loadTexture(
    const std::string& id,
    const std::function<std::unique_ptr<Image>()>& imageProvider)
{
    auto it = g_cache.textureCache.find(id);
    if (it == g_cache.textureCache.end()) {
        auto image = imageProvider();
        Texture texture(*image);
        g_cache.textureCache[id] = texture;
        return texture;
    }
    return it->second;
}

}
