#pragma once

namespace gamebase {

struct Size {
    Size() : width(0), height(0) {}
    Size(float w, float h) : width(w), height(h) {}

    float width;
    float height;
};

}
