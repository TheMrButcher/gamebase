/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/app/SimpleApplication.h>
#include <gamebase/impl/skin/impl/FullscreenPanelSkin.h>

namespace gamebase { namespace impl {

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

} }
