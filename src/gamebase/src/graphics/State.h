#pragma once

#include <gamebase/graphics/OneColorProgram.h>
#include <gamebase/graphics/LineProgram.h>
#include <gamebase/graphics/TextureProgram.h>

namespace gamebase {

struct State {
    int width;
    int height;

    struct {
        OneColorProgram oneColor;
        LineProgram line;
        TextureProgram texture;
    } programs;
};

void initState(int width, int height);
const State& state();

}
