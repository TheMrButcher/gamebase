/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/PatternProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

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

} }
