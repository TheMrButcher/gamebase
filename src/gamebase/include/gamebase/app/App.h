/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/AppHelpers.h>
#include <gamebase/app/Input.h>
#include <gamebase/ui/Layout.h>

namespace gamebase {

class App {
public:
    virtual void load() {}
    virtual void processInput() {}
    virtual void move() {}

    Layout design;
    Input input;
    
    void close();
    void setWindowTitle(const std::string& title);
    void setConfig(const std::string& name);
    void setDesign(const std::string& name);

    bool init(int* argc, char** argv);
    void run();

    GAMEBASE_DEFINE_APP_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline void App::close() { m_impl->close(); }
inline void App::setWindowTitle(const std::string& title) { m_impl->setWindowTitle(title); }
inline void App::setConfig(const std::string& name) { m_impl->setConfigName(name); }
inline void App::setDesign(const std::string& name) { m_impl->setDesignName(name); }
inline bool App::init(int* argc, char** argv) { return m_impl->init(argc, argv); }
inline void App::run() { m_impl->run(); }

}
