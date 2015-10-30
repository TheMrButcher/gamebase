#include <stdafx.h>
#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/FullscreenPanelSkin.h>

namespace gamebase {

SimpleApplication::SimpleApplication()
    : Application()
{}

void SimpleApplication::initView()
{
    m_view = std::make_shared<Panel>(
        nullptr,
        std::make_shared<FullscreenPanelSkin>(Color(1, 1, 1, 1)));
    ViewController::initView();
    activateController(this);
}

}
