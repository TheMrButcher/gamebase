/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "GlobalResources.h"
#include "Config.h"
#include "src/impl/text/ConversionInternal.h"
#include <iostream>

namespace gamebase { namespace impl {
namespace {
GlobalResources globalRes;
}

void loadGlobalResources()
{
    const auto& conf = config();
    for (auto path : conf.fontsPath)
        globalRes.fontStorage.load(path);
    globalRes.fontStorage.prepare();
    initConversionMaps();
}

GlobalResources& globalResources()
{
    return globalRes;
}

} }
