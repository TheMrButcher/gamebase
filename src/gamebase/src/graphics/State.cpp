#include <stdafx.h>
#include "State.h"
#include <memory>
#include <iostream>

namespace gamebase {
namespace {
std::unique_ptr<State> globalState;
}

void initState(int width, int height)
{
    std::cout << "Loading graphics... ";
    globalState.reset(new State);
    globalState->width = width;
    globalState->height = height;
    globalState->programs.oneColor.load();
    globalState->programs.line.load();
    globalState->programs.texture.load();
    std::cout << "Done" << std::endl;
}

const State& state()
{
    return *globalState;
}

}
