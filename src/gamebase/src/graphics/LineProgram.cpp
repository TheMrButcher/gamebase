#include <stdafx.h>
#include <gamebase/graphics/LineProgram.h>
#include "State.h"
#include "src/core/Config.h"

namespace gamebase {

LineProgram::LineProgram()
    : OneColorProgramBase(
        "Line",
        config().shadersPath + "Line.vs",
        config().shadersPath + "Line.fs")
{
    m_attrs.add(GLAttributes::Distance, 1);
    m_attrs.add(GLAttributes::ExtVec, 2);
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
    return state().programs.line;
}

}
