/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/ProgramBase.h>

namespace gamebase { namespace impl {

class GAMEBASE_API MultiColorProgram : public ProgramBase {
public:
    MultiColorProgram();
};

GAMEBASE_API const MultiColorProgram& multiColorProgram();

} }
