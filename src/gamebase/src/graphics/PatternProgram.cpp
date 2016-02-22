#include <stdafx.h>
#include <gamebase/graphics/PatternProgram.h>
#include "State.h"
#include "src/core/Config.h"

namespace gamebase {

PatternProgram::PatternProgram()
    : OneColorProgramBase(
        "Pattern",
        config().shadersPath + "Pattern.vs",
        config().shadersPath + "Pattern.fs")
{
    m_attrs.add("textureCoords", 2);
}

void PatternProgram::resetUniforms() const
{
    OneColorProgramBase::resetUniforms();
    texture = GLTexture();
    texCoordsOffset = Vec2();
}

void PatternProgram::locateUniforms()
{
    OneColorProgramBase::locateUniforms();
    m_texCoordsOffsetUniformLocation = locateUniform("uTexCoordsOffset");
}

void PatternProgram::loadUniforms() const
{
    OneColorProgramBase::loadUniforms();
    texture.bind();
    glUniform2f(m_texCoordsOffsetUniformLocation, texCoordsOffset.x, texCoordsOffset.y);
}

const PatternProgram& patternProgram()
{
    const auto& program = state().programs.pattern;
    program.activate();
    program.resetUniforms();
    return program;
}

}
