#pragma once

#include <gamebase/engine/AnimationManager.h>
#include <unordered_set>
#include <functional>
#include <vector>
#include <memory>

namespace gamebase {

struct GlobalTemporary {
    std::vector<std::function<void()>> delayedTasks;
    std::unordered_set<const AnimationManager*> currentAnimations;
};

extern GlobalTemporary g_temp;

}
