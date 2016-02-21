#include <stdafx.h>
#include <gamebase/graphics/GLProgram.h>
#include <gamebase/utils/Exception.h>

namespace gamebase {

GLProgram::GLProgram(
    const std::string& name,
    const std::string& vertexShaderName,
    const std::string& fragmentShaderName)
    : m_name(name)
    , m_vertexShader(GL_VERTEX_SHADER, vertexShaderName)
    , m_fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderName)
    , m_id(0)
    , m_loaded(false)
{}

GLuint GLProgram::load()
{
    m_id = glCreateProgram();
    if (m_id == 0)
        THROW_EX() << "Can't create program " << m_name;

    glAttachShader(m_id, m_vertexShader.load());
    glAttachShader(m_id, m_fragmentShader.load());
    glLinkProgram(m_id);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar buf[1024];
        glGetProgramInfoLog(m_id, sizeof(buf), NULL, buf);
        THROW_EX() << "Error while linking program " << m_name << ": " << buf;
    }

    locateUniforms();
    m_attrs.locate(m_id, m_name);
    m_loaded = true;
    return m_id;
}

void GLProgram::activate() const
{
    if (!m_loaded)
        return;

    if (!m_id)
        THROW_EX() << "Can't activate program " << m_name << ", cause it's not loaded";
    glUseProgram(m_id);
}

void GLProgram::draw(const VertexBuffer& vbo, const IndexBuffer& ibo) const
{
    if (!m_loaded)
        return;

    if (ibo.size() > 65536 || ibo.size() % 3 != 0)
        THROW_EX() << "Can't draw program " << m_name
            << ". Wrong size of IndexBuffer: " << ibo.size();

    loadUniforms();

    vbo.bind();
    ibo.bind();
    
    m_attrs.activate();
    glDrawElements(GL_TRIANGLES, ibo.size(), GL_UNSIGNED_SHORT, NULL);
    m_attrs.disable();
}

GLuint GLProgram::locateUniform(const std::string& name) const
{
    GLuint uniformLocation = glGetUniformLocation(m_id, name.c_str());
    if (uniformLocation == 0xFFFFFFFF)
        THROW_EX() << "Can't locate uniform " << name << " in program " << m_name;
    return uniformLocation;
}

}
