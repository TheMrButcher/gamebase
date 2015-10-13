#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/graphics/typedefs.h>
#include <memory>

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

}
