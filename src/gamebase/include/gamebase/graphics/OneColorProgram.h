#pragma once

#include <gamebase/graphics/OneColorProgramBase.h>

namespace gamebase {

class GAMEBASE_API OneColorProgram : public OneColorProgramBase {
public:
    OneColorProgram();
};

GAMEBASE_API const OneColorProgram& oneColorProgram();

}
