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
    globalRes.fontStorage.load(conf.fontsPath);
    initConversionMaps();
}

const GlobalResources& globalResources()
{
    return globalRes;
}

} }
