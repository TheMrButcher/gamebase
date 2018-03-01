/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/AppHelpers.h>
#include <gamebase/app/Input.h>
#include <gamebase/audio/AudioManager.h>
#include <gamebase/ui/Layout.h>

namespace gamebase {

class App {
public:
    virtual void load() {}
    virtual void process(Input input) {}
    virtual void move() {}
    virtual void onResize(Size size) {}
    virtual bool onClose() { return true; }

    Layout design;
    AudioManager audio;
    
    void close();
    void setWindowTitle(const std::string& title);
    void setConfig(const std::string& name);
    void setDesign(const std::string& name);
    void setSize(int w, int h);
    void setMinSize(int w, int h);
    void setMaxSize(int w, int h);

    bool init(int* argc, char** argv);
    void run();

    GAMEBASE_DEFINE_APP_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline void App::close() { m_impl->close(); }
inline void App::setWindowTitle(const std::string& title) { m_impl->setWindowTitle(title); }
inline void App::setConfig(const std::string& name) { m_impl->setConfigName(name); }
inline void App::setDesign(const std::string& name) { m_impl->setDesignName(name); }
inline void App::setSize(int w, int h) { m_impl->setWindowSize(static_cast<unsigned int>(w), static_cast<unsigned int>(h)); }
inline void App::setMinSize(int w, int h) { m_impl->setMinWindowSize(static_cast<unsigned int>(w), static_cast<unsigned int>(h)); }
inline void App::setMaxSize(int w, int h) { m_impl->setMaxWindowSize(static_cast<unsigned int>(w), static_cast<unsigned int>(h)); }
inline bool App::init(int* argc, char** argv) { return m_impl->init(argc, argv); }
inline void App::run() { m_impl->run(); }

}
