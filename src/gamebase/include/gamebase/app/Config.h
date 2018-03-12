/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/tools/Json.h>

namespace gamebase {

enum ScreenMode {
    WindowMode,
    FullscreenMode,
    WindowNoResizeMode
};

class GAMEBASE_API Config {
public:
    int width() const;
    int height() const;
    void setWidth(int w);
    void setHeight(int h);
    void setSize(int w, int h);

    int minWidth() const;
    int minHeight() const;
    void setMinWidth(int w);
    void setMinHeight(int h);
    void setMinSize(int w, int h);

    int maxWidth() const;
    int maxHeight() const;
    void setMaxWidth(int w);
    void setMaxHeight(int h);
    void setMaxSize(int w, int h);

    bool isConsoleVisible() const;
    void setConsoleVisible(bool value);

    std::string windowTitle() const;
    void setWindowTitle(const std::string& title);

    ScreenMode screenMode() const;
    void setScreenMode(ScreenMode mode);

    Json toJson() const;
    void fromJson(const Json& json);
};

GAMEBASE_API extern Config config;

}
