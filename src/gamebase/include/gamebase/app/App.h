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
    
    void stop();
    void setWindowTitle(const std::string& name);
    void setConfig(const std::string& name);
    void setDesign(const std::string& name);

    bool init(int* argc, char** argv);
    void run();

    GAMEBASE_DEFINE_APP_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline void App::stop() { m_impl->stop(); }
inline void App::setWindowTitle(const std::string& name) { m_impl->setWindowName(name); }
inline void App::setConfig(const std::string& name) { m_impl->setConfigName(name); }
inline void App::setDesign(const std::string& name) { m_impl->setDesignName(name); }
inline bool App::init(int* argc, char** argv) { return m_impl->init(argc, argv); }
inline void App::run() { m_impl->run(); }

}
