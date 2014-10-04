#include <stdafx.h>
#include <gamebase/graphics/OneColorProgramBase.h>

namespace gamebase {

OneColorProgramBase::OneColorProgramBase(
    const std::string& name,
    const std::string& vertexShaderName,
    const std::string& fragmentShaderName)
    : ProgramBase(name, vertexShaderName, fragmentShaderName)
{}

void OneColorProgramBase::locateUniforms()
{
    ProgramBase::locateUniforms();
    m_colorUniformLocation = locateUniform("uColor");
}

void OneColorProgramBase::loadUniforms() const
{
    ProgramBase::loadUniforms();
    glUniform4f(m_colorUniformLocation, color.r, color.g, color.b, color.a);
}

}
