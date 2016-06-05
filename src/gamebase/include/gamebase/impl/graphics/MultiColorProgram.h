#pragma once

#include <gamebase/impl/graphics/ProgramBase.h>

namespace gamebase { namespace impl {

class GAMEBASE_API MultiColorProgram : public ProgramBase {
public:
    MultiColorProgram();
};

GAMEBASE_API const MultiColorProgram& multiColorProgram();

} }
