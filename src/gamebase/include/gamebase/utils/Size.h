#pragma once

namespace gamebase {

struct Size {
    Size() : width(0), height(0) {}
    Size(unsigned int w, unsigned int h) : width(w), height(h) {}

    unsigned int width;
    unsigned int height;
};

}
