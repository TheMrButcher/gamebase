#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/text/FontStorage.h>

namespace gamebase { namespace impl {

struct GlobalResources {
    FontStorage fontStorage;
};

void loadGlobalResources();
const GlobalResources& globalResources();

} }
