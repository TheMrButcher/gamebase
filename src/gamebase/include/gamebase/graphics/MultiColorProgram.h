#pragma once

#include <gamebase/graphics/ProgramBase.h>

namespace gamebase {

class GAMEBASE_API MultiColorProgram : public ProgramBase {
public:
    MultiColorProgram();
};

GAMEBASE_API const MultiColorProgram& multiColorProgram();

}
