/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "src/impl/tools/TimerSharedState.h"
#include <gamebase/impl/anim/AnimationManager.h>
#include <unordered_set>
#include <functional>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

struct GlobalTemporary {
    std::vector<std::function<void()>> delayedTasks;
	std::vector<std::weak_ptr<TimerSharedState>> timers;
    std::unordered_set<const AnimationManager*> currentAnimations;
};

extern GlobalTemporary g_temp;

} }
