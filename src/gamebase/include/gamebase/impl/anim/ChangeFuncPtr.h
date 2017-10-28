/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/ChangeFunc.h>

namespace gamebase { namespace impl {

typedef float(*ChangeFuncPtr)(float);

GAMEBASE_API ChangeFuncPtr getChangeFuncPtr(ChangeFunc::Type type);

} }
