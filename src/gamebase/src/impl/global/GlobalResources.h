/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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
