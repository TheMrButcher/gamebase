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
    std::vector<Registrar> leftRegistrars;
    for (auto it = m_registrars.begin(); it != m_registrars.end(); ++it) {
        auto& reg = *it;
        if (!reg(m_pubApp, m_registerRoot.get(), false))
            leftRegistrars.push_back(reg);
    }

    m_pubApp->load();
    
    size_t numOfBrokenObjs = 0;
    for (auto it = leftRegistrars.begin(); it != leftRegistrars.end(); ++it) {
        try {
            (*it)(m_pubApp, m_registerRoot.get(), true);
        } catch (Exception& ex) {
            std::cerr << "Loading error #" << numOfBrokenObjs++ << ": " << ex.what() << std::endl;
        }
    }
    if (numOfBrokenObjs > 0)
        THROW_EX() << "Can't finish initing application. Found "
            << numOfBrokenObjs << " incorrect objects";
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
