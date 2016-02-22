#include <stdafx.h>
#include <gamebase/graphics/GLTexture.h>
#include <gamebase/utils/Exception.h>
#include "src/core/GlobalCache.h"
#include <functional>

namespace gamebase {

namespace {
const GLint WRAP_MODES[] = {
    GL_CLAMP_TO_EDGE,
    GL_REPEAT,
    GL_MIRRORED_REPEAT
};

GLTexture loadTexture(
    const TextureKey& key,
    const std::function<std::unique_ptr<Image>()>& imageProvider)
{
    auto it = g_cache.textureCache.find(key);
    if (it == g_cache.textureCache.end()) {
        auto image = imageProvider();
        GLTexture texture(*image, key.wrapX, key.wrapY);
        g_cache.textureCache[key] = texture;
        return texture;
    }
    return it->second;
}
}

GLTexture::GLTexture(const Image& image)
{
    load(image, Clamp, Clamp);
}

GLTexture::GLTexture(const Image& image, WrapMode wrapX, WrapMode wrapY)
{
    load(image, wrapX, wrapY);
}

void GLTexture::bind() const
{
    if (m_size.width == 0 || m_size.height == 0 || !m_id)
        THROW_EX() << "Can't bind empty Texture";
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *m_id);
}

void GLTexture::load(const Image& image, WrapMode wrapX, WrapMode wrapY)
{
    m_size = image.size;
    auto* id = new GLuint(0);
    m_id.reset(id, std::bind(glDeleteTextures, 1, id));
    glGenTextures(1, m_id.get());
    glBindTexture(GL_TEXTURE_2D, *m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WRAP_MODES[wrapX]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WRAP_MODES[wrapY]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.size.width, image.size.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, &image.data.front());
}

GLTexture loadTexture(
    const std::string& id,
    const std::function<std::unique_ptr<Image>()>& imageProvider)
{
    return loadTexture(TextureKey(id), imageProvider);
}

GLTexture loadPattern(
    const std::string& id,
    GLTexture::WrapMode wrapX,
    GLTexture::WrapMode wrapY,
    const std::function<std::unique_ptr<Image>()>& imageProvider)
{
    return loadTexture(TextureKey(id, wrapX, wrapY), imageProvider);
}

}
