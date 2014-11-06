#include <stdafx.h>
#include <gamebase/graphics/ProgramBase.h>

namespace gamebase {

ProgramBase::ProgramBase(
    const std::string& name,
    const std::string& vertexShaderName,
    const std::string& fragmentShaderName)
    : GLProgram(name, vertexShaderName, fragmentShaderName)
{
    m_attrs.add(GLAttributes::Position, 2);
}

void ProgramBase::resetUniforms() const
{
    transform = Transform2();
}

void ProgramBase::locateUniforms()
{
    m_matrixUniformLocation = locateUniform("uMatrix");
    m_offsetUniformLocation = locateUniform("uOffset");
}

void ProgramBase::loadUniforms() const
{
    glUniformMatrix2fv(m_matrixUniformLocation, 1, GL_FALSE, transform.matrix.dataPtr());
    glUniform2f(m_offsetUniformLocation, transform.offset.x, transform.offset.y);
}

}
