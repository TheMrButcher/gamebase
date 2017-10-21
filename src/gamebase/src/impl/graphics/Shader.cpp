/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/Shader.h>
#include <gamebase/tools/FileIO.h>
#include <gamebase/tools/Exception.h>

namespace gamebase { namespace impl {

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

} }
