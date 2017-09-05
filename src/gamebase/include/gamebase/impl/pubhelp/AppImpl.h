/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/app/Application.h>

namespace gamebase {

class App;

namespace impl {

class GAMEBASE_API AppImpl : public Application {
public:
    AppImpl(App* app);

    void setDesignName(const std::string& name)
    {
        m_designName = name;
    }

    typedef std::function<void(App*, IRegistrable*)> Registrar;
    
    void addRegistrar(const Registrar& registrar)
    {
        m_registrars.push_back(registrar);
    }

    virtual void load() override;
    virtual void postload() override;
    virtual void move() override;
    virtual void processInput(const InputRegister& inputRegister) override;
	virtual void onResize(const Size& size) override;

private:
    App* m_pubApp;
    std::string m_designName;
    std::vector<Registrar> m_registrars;
};

GAMEBASE_API extern AppImpl* g_appImpl;

} }
