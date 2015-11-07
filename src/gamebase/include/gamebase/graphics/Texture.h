#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/graphics/typedefs.h>
#include <memory>
#include <functional>

namespace gamebase {

class GAMEBASE_API Texture {
public:
    Texture() {}

    Texture(const Image& image);

    GLuint id() const { return *m_id; }
    const Size& size() const { return m_size; }

    void bind() const;

private:
    std::shared_ptr<GLuint> m_id;
    Size m_size;
};

static const char* DEFAULT_IMAGE_ID = "SYSDEF";
static const char* WHITE_RECT_IMAGE_ID = "SYSWHITE";

GAMEBASE_API Texture loadTexture(
    const std::string& id,
    const std::function<std::unique_ptr<Image>()>& imageProvider);

}
