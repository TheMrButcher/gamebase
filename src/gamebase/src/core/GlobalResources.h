#pragma once

#include <gamebase/engine/IObject.h>
#include <gamebase/text/FontStorage.h>

namespace gamebase {

struct GlobalResources {
    FontStorage fontStorage;
};

void loadGlobalResources();
const GlobalResources& globalResources();

}
