/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/OneColorProgramBase.h>

namespace gamebase { namespace impl {

class GAMEBASE_API OneColorProgram : public OneColorProgramBase {
public:
    OneColorProgram();
};

GAMEBASE_API const OneColorProgram& oneColorProgram();

} }
