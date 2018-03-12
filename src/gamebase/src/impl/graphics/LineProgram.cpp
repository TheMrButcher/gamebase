/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/LineProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

LineProgram::LineProgram()
    : OneColorProgramBase(
        "Line",
        config().shadersPath + "Line.vs",
        config().shadersPath + "Line.fs")
{
    m_attrs.add("distance", 1);
    m_attrs.add("extVec", 2);
}

void LineProgram::locateUniforms()
{
    OneColorProgramBase::locateUniforms();
    m_extScaleLocation = locateUniform("uExtScale");
}

void LineProgram::loadUniforms() const
{
    OneColorProgramBase::loadUniforms();
    const State& curState = state();
    float extScale = 1.0f / (transform.matrix.scaleX() * curState.width);
    glUniform1f(m_extScaleLocation, extScale);
}

const LineProgram& lineProgram()
{
    const auto& program = state().programs.line;
    program.activate();
    program.resetUniforms();
    return program;
}

} }
