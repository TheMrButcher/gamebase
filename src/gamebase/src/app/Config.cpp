/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/app/Config.h>
#include <gamebase/impl/app/Config.h>
#include <gamebase/impl/app/Application.h>
#include "src/impl/global/Config.h"
#include <limits>
#include <fstream>

namespace gamebase {

namespace {
void updateConfig(Config& conf, const std::function<void(Json&)>& updater)
{
    auto json = conf.toJson();
    updater(json);
    conf.fromJson(json);

    auto text = json.serialize(StyledJsonFormat);
    std::ofstream file(impl::app->configName());
    file << text;
}
}

int Config::width() const
{
    return static_cast<int>(impl::config().windowSize.w);
}

int Config::height() const
{
    return static_cast<int>(impl::config().windowSize.h);
}

void Config::setWidth(int w)
{
    if (w == width())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["width"] = static_cast<unsigned int>(w);
    });
}

void Config::setHeight(int h)
{
    if (h == height())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["height"] = static_cast<unsigned int>(h);
    });
}

void Config::setSize(int w, int h)
{
    if (w == width() && h == height())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["width"] = static_cast<unsigned int>(w);
        json["height"] = static_cast<unsigned int>(h);
    });
}

int Config::minWidth() const
{
    return impl::config().minWindowSize
        ? static_cast<int>(impl::config().minWindowSize->w)
        : 0;
}

int Config::minHeight() const
{
    return impl::config().minWindowSize
        ? static_cast<int>(impl::config().minWindowSize->h)
        : 0;
}

void Config::setMinWidth(int w)
{
    if (w == minWidth())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["minWidth"] = static_cast<unsigned int>(w);
    });
}

void Config::setMinHeight(int h)
{
    if (h == minHeight())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["minHeight"] = static_cast<unsigned int>(h);
    });
}

void Config::setMinSize(int w, int h)
{
    if (w == minWidth() && h == minHeight())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["minWidth"] = static_cast<unsigned int>(w);
        json["minHeight"] = static_cast<unsigned int>(h);
    });
}

int Config::maxWidth() const
{
    return impl::config().maxWindowSize
        ? static_cast<int>(impl::config().maxWindowSize->w)
        : std::numeric_limits<int>::max();
}

int Config::maxHeight() const
{
    return impl::config().maxWindowSize
        ? static_cast<int>(impl::config().maxWindowSize->h)
        : std::numeric_limits<int>::max();
}

void Config::setMaxWidth(int w)
{
    if (w == maxWidth())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["maxWidth"] = static_cast<unsigned int>(w);
    });
}

void Config::setMaxHeight(int h)
{
    if (h == maxHeight())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["maxHeight"] = static_cast<unsigned int>(h);
    });
}

void Config::setMaxSize(int w, int h)
{
    if (w == maxWidth() && h == maxHeight())
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["maxWidth"] = static_cast<unsigned int>(w);
        json["maxHeight"] = static_cast<unsigned int>(h);
    });
}

bool Config::isConsoleVisible() const
{
    return impl::config().showConsole;
}

void Config::setConsoleVisible(bool value)
{
    if (isConsoleVisible() == value)
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["showConsole"] = value;
    });
}

std::string Config::windowTitle() const
{
    return impl::config().windowTitle;
}

void Config::setWindowTitle(const std::string& title)
{
    if (windowTitle() == title)
        return;
    updateConfig(*this, [=](Json& json)
    {
        json["windowTitle"] = title;
    });
}

ScreenMode Config::screenMode() const
{
    switch (impl::config().mode) {
    case impl::GraphicsMode::Window: return WindowMode;
    case impl::GraphicsMode::Fullscreen: return FullscreenMode;
    case impl::GraphicsMode::WindowNoResize: return WindowNoResizeMode;
    default: return WindowMode;
    }
}

void Config::setScreenMode(ScreenMode mode)
{
    if (mode == screenMode())
        return;
    updateConfig(*this, [=](Json& json)
    {
        switch (mode) {
        case WindowMode: json["mode"] = "window"; break;
        case FullscreenMode: json["mode"] = "fullscreen"; break;
        case WindowNoResizeMode: json["mode"] = "noresize"; break;
        }
    });
}

Json Config::toJson() const
{
    return loadJsonFromString(impl::configAsString());
}

void Config::fromJson(const Json& json)
{
    impl::configurateFromString(json.serialize(), false);
}

Config config;

}
