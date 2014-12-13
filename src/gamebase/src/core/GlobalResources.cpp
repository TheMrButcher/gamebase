#include <stdafx.h>
#include "GlobalResources.h"
#include "Config.h"
#include <iostream>

namespace gamebase {
namespace {
GlobalResources globalRes;
}

void loadGlobalResources()
{
    const auto& conf = config();
    globalRes.fontStorage.load(conf.fontsPath);
}

const GlobalResources& globalResources()
{
    return globalRes;
}

}