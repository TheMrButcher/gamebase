#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/graphics/typedefs.h>
#include <memory>
#include <functional>

namespace gamebase {

class GAMEBASE_API GLTexture {
public:
    enum WrapMode {
        Clamp,
        Repeat,
        RepeatMirrored
    };

    GLTexture() {}

    GLTexture(const Image& image);
    GLTexture(const Image& image, WrapMode wrapX, WrapMode wrapY);

    GLuint id() const { return m_id ? *m_id : 0; }
    const Size& size() const { return m_size; }

    void bind() const;

private:
    void load(const Image& image, WrapMode wrapX, WrapMode wrapY);

    std::shared_ptr<GLuint> m_id;
    Size m_size;
};

static const char* DEFAULT_IMAGE_ID = "SYSDEF";
static const char* WHITE_RECT_IMAGE_ID = "SYSWHITE";

GAMEBASE_API GLTexture loadTexture(
    const std::string& id,
    const std::function<std::unique_ptr<Image>()>& imageProvider);

GAMEBASE_API GLTexture loadPattern(
    const std::string& id,
    GLTexture::WrapMode wrapX,
    GLTexture::WrapMode wrapY,
    const std::function<std::unique_ptr<Image>()>& imageProvider);

}
