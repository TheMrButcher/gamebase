#pragma once

#include <functional>
#include <vector>
#include <memory>

namespace gamebase {

struct GlobalTemporary {
    std::vector<std::function<void()>> delayedTasks;
};

extern GlobalTemporary g_temp;

}
