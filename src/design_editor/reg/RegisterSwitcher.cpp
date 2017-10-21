/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RegisterSwitcher.h"
#include <reg/ResultBuilderRegisterSnapshot.h>
#include <reg/DesignViewBuilderRegisterSnapshot.h>

namespace gamebase { namespace editor {

RegisterSwitcher::RegType g_currentRegType;

RegisterSwitcher::RegisterSwitcher()
    : startingReg_(current())
{
    if (startingReg_ != DVBReg)
        DesignViewBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::~RegisterSwitcher()
{
    if (startingReg_ == ResultReg)
        ResultBuilderRegisterSnapshot::instance().apply();
}

void RegisterSwitcher::init()
{
    ResultBuilderRegisterSnapshot::instance().apply();
}

RegisterSwitcher::RegType RegisterSwitcher::current()
{
    return g_currentRegType;
}
} }
