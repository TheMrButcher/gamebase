#pragma once

#include <gamebase/graphics/OneColorProgram.h>
#include <gamebase/graphics/LineProgram.h>
#include <gamebase/graphics/TextureProgram.h>
#include <gamebase/graphics/PatternProgram.h>
#include <gamebase/graphics/MultiColorProgram.h>

namespace gamebase {

struct State {
    int width;
    int height;

    struct {
        OneColorProgram oneColor;
        LineProgram line;
        TextureProgram texture;
        PatternProgram pattern;
        MultiColorProgram multiColor;
    } programs;
};

void initState(int width, int height);
const State& state();

}
