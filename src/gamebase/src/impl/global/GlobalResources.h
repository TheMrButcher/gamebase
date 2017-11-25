/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/text/FontStorage.h>
#include <gamebase/impl/text/TextBank.h>

namespace gamebase { namespace impl {

struct GlobalResources {
    FontStorage fontStorage;
    TextBank textBank;
};

void loadGlobalResources();
GlobalResources& globalResources();

} }
