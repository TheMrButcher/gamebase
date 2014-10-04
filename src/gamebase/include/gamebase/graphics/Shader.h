#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/graphics/typedefs.h>
#include <boost/noncopyable.hpp>
#include <string>

namespace gamebase {

class GAMEBASE_API Shader : boost::noncopyable {
public:
    Shader(GLenum type, const std::string& name)
        : m_type(type)
        , m_name(name)
        , m_id(0)
    {}

    const std::string& name() const { return m_name; }
    GLuint id() const { return m_id; }

    GLuint load();

private:
    GLenum m_type;
    std::string m_name;
    GLuint m_id;
};

}
