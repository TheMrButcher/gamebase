/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/pubhelp/AppImpl.h>
#include <gamebase/impl/skin/impl/FullscreenPanelSkin.h>
#include <gamebase/app/App.h>
#include <gamebase/serial/LoadObj.h>

namespace gamebase { namespace impl {

AppImpl* g_appImpl = nullptr;

AppImpl::AppImpl(App* app)
    : m_pubApp(app)
    , m_designName("Design.json")
{
    g_appImpl = this;
}

void AppImpl::load()
{
    m_view = std::make_shared<Panel>(
        nullptr,
        std::make_shared<FullscreenPanelSkin>(Color(1, 1, 1, 1)));
    m_pubApp->design = loadObj<Layout>(m_designName);
    m_view->addObject(m_pubApp->design.getImpl()->getInternalObj().getShared());
    activateController(this);
}

void AppImpl::postload()
{
    size_t numOfBrokenObjs = 0;
    for (auto it = m_registrars.begin(); it != m_registrars.end(); ++it) {
        auto& reg = *it;
        try {
            reg(m_pubApp, m_registerRoot.get());
        } catch (Exception& ex) {
            std::cerr << "Loading error #" << numOfBrokenObjs++ << ": " << ex.what() << std::endl;
        }
    }
    if (numOfBrokenObjs > 0)
        THROW_EX() << "Can't finish initing application. Can't find "
            << numOfBrokenObjs << " objects in design";

    m_pubApp->load();
}

void AppImpl::move()
{
    m_pubApp->move();
}

void AppImpl::processInput(const InputRegister&)
{
    m_pubApp->processInput();
}

} }
