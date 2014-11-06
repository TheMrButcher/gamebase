#include <stdafx.h>
#include <gamebase/graphics/OneColorProgram.h>
#include "State.h"
#include "src/core/Config.h"

namespace gamebase {

OneColorProgram::OneColorProgram()
    : OneColorProgramBase(
        "OneColor",
        config().shadersPath + "OneColor.vs",
        config().shadersPath + "OneColor.fs")
{}

const OneColorProgram& oneColorProgram()
{
    const auto& program = state().programs.oneColor;
    program.activate();
    program.resetUniforms();
    return program;
}

}
