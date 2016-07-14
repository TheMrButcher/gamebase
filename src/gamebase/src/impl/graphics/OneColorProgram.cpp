/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/OneColorProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

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

} }
