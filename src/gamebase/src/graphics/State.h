#pragma once

#include <gamebase/graphics/OneColorProgram.h>
#include <gamebase/graphics/LineProgram.h>

namespace gamebase {

struct State {
    int width;
    int height;

    struct {
        OneColorProgram oneColor;
        LineProgram line;
    } programs;
};

void initState(int width, int height);
const State& state();

}
