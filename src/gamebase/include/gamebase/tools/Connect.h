/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/app/AppType.h>
#include <functional>

#define connect0(OBJECT_NAME, METHOD_NAME) \
    OBJECT_NAME.setCallback(std::bind(&GAMEBASE_APP_TYPE::METHOD_NAME, this))

#define connect1(OBJECT_NAME, METHOD_NAME, ARG1) \
    OBJECT_NAME.setCallback(std::bind(&GAMEBASE_APP_TYPE::METHOD_NAME, this, ARG1))

#define connect2(OBJECT_NAME, METHOD_NAME, ARG1, ARG2) \
    OBJECT_NAME.setCallback(std::bind(&GAMEBASE_APP_TYPE::METHOD_NAME, this, ARG1, ARG2))

#define connect3(OBJECT_NAME, METHOD_NAME, ARG1, ARG2, ARG3) \
    OBJECT_NAME.setCallback(std::bind(&GAMEBASE_APP_TYPE::METHOD_NAME, this, ARG1, ARG2, ARG3))

#define connect4(OBJECT_NAME, METHOD_NAME, ARG1, ARG2, ARG3, ARG4) \
    OBJECT_NAME.setCallback(std::bind(&GAMEBASE_APP_TYPE::METHOD_NAME, this, ARG1, ARG2, ARG3, ARG4))
