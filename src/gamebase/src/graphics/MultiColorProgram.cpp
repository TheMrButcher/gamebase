#include <stdafx.h>
#include <gamebase/graphics/MultiColorProgram.h>
#include "State.h"
#include "src/core/Config.h"

namespace gamebase {

MultiColorProgram::MultiColorProgram()
    : ProgramBase(
        "MultiColor",
        config().shadersPath + "MultiColor.vs",
        config().shadersPath + "MultiColor.fs")
{
    m_attrs.add("color", 4);
}

const MultiColorProgram& multiColorProgram()
{
    const auto& program = state().programs.multiColor;
    program.activate();
    program.resetUniforms();
    return program;
}

}
