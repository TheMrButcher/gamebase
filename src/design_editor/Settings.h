#pragma once

#include <gamebase/graphics/GraphicsMode.h>
#include <string>

namespace gamebase { namespace editor { namespace settings {

extern bool isInterfaceExtended;
extern std::string workDir;
extern std::string imagesDir;
extern std::string mainConf;
extern std::string designedObjConf;

void init();
void formMainConfig(int width, int height, GraphicsMode::Enum mode);
void formDesignedObjConfig();

} } }
