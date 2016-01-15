#include <stdafx.h>
#include "State.h"
#include <gamebase/graphics/Clipping.h>
#include <gamebase/math/Math.h>

namespace gamebase {

namespace {
bool isClipperEnabled = false;
std::vector<BoundingBox> clipBoxes;
}

void pushClipBox(const Transform2& pos, const BoundingBox& box)
{
    if (!isClipperEnabled) {
        glEnable(GL_SCISSOR_TEST);
        isClipperEnabled = true;
    }

    const State& curState = state();
    auto fullTransform = pos * Transform2(
        ScalingMatrix2(0.5f * curState.width, 0.5f * curState.height),
        Vec2(0.5f * curState.width, 0.5f * curState.height));
    auto clipBox = box.transformed(fullTransform);
    if (!clipBoxes.empty())
        clipBox = clipBox.intersectWith(clipBoxes.back());
    glScissor(
        round(clipBox.bottomLeft.x), round(clipBox.bottomLeft.y),
        uround(clipBox.width()), uround(clipBox.height()));
    clipBoxes.push_back(clipBox);
}

void popClipBox()
{
    clipBoxes.pop_back();
    if (clipBoxes.empty()) {
        disableClipping();
    } else {
        auto clipBox = clipBoxes.back();
        glScissor(
            round(clipBox.bottomLeft.x), round(clipBox.bottomLeft.y),
            uround(clipBox.width()), uround(clipBox.height()));
    }
}

void resetClipper()
{
    clipBoxes.clear();
    disableClipping();
}

void disableClipping()
{
    if (isClipperEnabled) {
        glDisable(GL_SCISSOR_TEST);
        isClipperEnabled = false;
    }
}

}
