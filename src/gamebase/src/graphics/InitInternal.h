#pragma once

namespace gamebase {
static const char* DEFAULT_CONFIG_NAME = "config.json";

void initWindowModeInternal(int* argc, char** argv,
    int width, int height, const std::string& name,
    int posX, int posY);

void initGameModeInternal(int* argc, char** argv, int width, int height);
}
