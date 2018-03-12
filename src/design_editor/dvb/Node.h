/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/ButtonKey.h>
#include <dvb/Properties.h>
#include <map>
#include <functional>

namespace gamebase { namespace editor {
struct Node {
    std::map<ButtonKey::Enum, std::function<void()>> callbacks;
    std::shared_ptr<Properties> props;
};
} }
