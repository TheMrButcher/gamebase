/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <functional>

#define connect(OBJECT_NAME, FUNCTION_NAME, ...) \
    OBJECT_NAME.setCallback([=]() { FUNCTION_NAME(##__VA_ARGS__); })
