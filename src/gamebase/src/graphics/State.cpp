#include <stdafx.h>
#include "State.h"
#include <memory>

namespace gamebase {
namespace {
std::unique_ptr<State> globalState;
}

void initState(int width, int height)
{
    globalState.reset(new State);
    globalState->width = width;
    globalState->height = height;
    globalState->programs.oneColor.load();
    globalState->programs.line.load();
}

const State& state()
{
    return *globalState;
}

}
