/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/MultiColorProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

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

} }
