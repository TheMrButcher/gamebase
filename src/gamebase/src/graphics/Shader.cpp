#include <stdafx.h>
#include <gamebase/graphics/Shader.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

GLuint Shader::load()
{
    std::string shaderStr = loadTextFile(m_name);
    m_id = glCreateShader(m_type);
    if (m_id == 0)
        THROW_EX() << "Can't create shader " << m_name;
        
    const char* data = &shaderStr.front();
    GLint len = (GLint)shaderStr.length();
    glShaderSource(m_id, 1, &data, &len);
    glCompileShader(m_id);

    GLint success;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar buf[1024];
        glGetShaderInfoLog(m_id, sizeof(buf), NULL, buf);
        THROW_EX() << "Error while compiling shader " << m_name << ": " << buf;
    }

    return m_id;
}

}
