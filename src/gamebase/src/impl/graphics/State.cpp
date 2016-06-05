#include <stdafx.h>
#include "State.h"
#include <memory>
#include <iostream>

namespace gamebase { namespace impl {
namespace {
std::unique_ptr<State> globalState;

void loadProgram(GLProgram& program)
{
    try {
        program.load();
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading GL-program: " << program.name()
            << ". Reason: " << ex.what()
            << ". Rendering using this program will be skipped" << std::endl;
        return;
    }
    std::cout << "Successfully loaded GL-program: " << program.name() << std::endl;
}
}

void initState(int width, int height)
{
    std::cout << "Loading graphics... " << std::endl;
    globalState.reset(new State);
    globalState->width = width;
    globalState->height = height;
    loadProgram(globalState->programs.oneColor);
    loadProgram(globalState->programs.line);
    loadProgram(globalState->programs.texture);
    loadProgram(globalState->programs.pattern);
    loadProgram(globalState->programs.multiColor);
    std::cout << "Done" << std::endl;
}

const State& state()
{
    return *globalState;
}

} }
