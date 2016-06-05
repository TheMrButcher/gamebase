#pragma once

#include <gamebase/impl/anim/AnimationManager.h>
#include <unordered_set>
#include <functional>
#include <vector>
#include <memory>

namespace gamebase { namespace impl {

struct GlobalTemporary {
    std::vector<std::function<void()>> delayedTasks;
    std::unordered_set<const AnimationManager*> currentAnimations;
};

extern GlobalTemporary g_temp;

} }
