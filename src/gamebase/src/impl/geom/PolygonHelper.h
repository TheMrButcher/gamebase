/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/math/Vector2.h>
#include <stddef.h>
#include <vector>

namespace gamebase { namespace impl {

std::vector<uint16_t> triangulate(const std::vector<std::vector<Vec2>>& vertices);

} }
