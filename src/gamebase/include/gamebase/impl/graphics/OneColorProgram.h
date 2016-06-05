#pragma once

#include <gamebase/impl/graphics/OneColorProgramBase.h>

namespace gamebase { namespace impl {

class GAMEBASE_API OneColorProgram : public OneColorProgramBase {
public:
    OneColorProgram();
};

GAMEBASE_API const OneColorProgram& oneColorProgram();

} }
