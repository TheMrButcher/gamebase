/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/GraphicsMode.h>
#include <string>

namespace gamebase { namespace editor { namespace settings {

extern bool isInterfaceExtended;
extern std::string workDir;
extern std::string imagesDir;
extern std::string mainConf;
extern std::string designedObjConf;
extern bool isBackupEnabled;
extern bool isComplexBoxMode;
bool isComplexLayerMode();
void setComplexLayerMode(bool value);

void init();
void formMainConfig(int width, int height, impl::GraphicsMode::Enum mode);
void formDesignedObjConfig();

} } }
