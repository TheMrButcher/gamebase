/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/TimeState.h>

namespace gamebase {

inline float timeDelta() { return impl::TimeState::realTime().delta / 1000.0f; }

}
