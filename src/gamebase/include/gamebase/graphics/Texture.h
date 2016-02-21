#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/graphics/typedefs.h>
#include <memory>
#include <functional>

namespace gamebase {

class GAMEBASE_API Texture {
public:
    enum WrapMode {
        Clamp,
        Repeat,
        RepeatMirrored
    };

    Texture() {}

    Texture(const Image& image);
    Texture(const Image& image, WrapMode wrapX, WrapMode wrapY);

    GLuint id() const { return *m_id; }
    const Size& size() const { return m_size; }

    void bind() const;

private:
    void load(const Image& image, WrapMode wrapX, WrapMode wrapY);

    std::shared_ptr<GLuint> m_id;
    Size m_size;
};

static const char* DEFAULT_IMAGE_ID = "SYSDEF";
static const char* WHITE_RECT_IMAGE_ID = "SYSWHITE";

GAMEBASE_API Texture loadTexture(
    const std::string& id,
    const std::function<std::unique_ptr<Image>()>& imageProvider);

GAMEBASE_API Texture loadPattern(
    const std::string& id,
    Texture::WrapMode wrapX,
    Texture::WrapMode wrapY,
    const std::function<std::unique_ptr<Image>()>& imageProvider);

}
