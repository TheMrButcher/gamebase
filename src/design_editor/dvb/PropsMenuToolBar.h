/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/ButtonKey.h>
#include <gamebase/ui/Button.h>
#include <gamebase/ui/Layout.h>

namespace gamebase { namespace editor {

class PropsMenuToolBar {
public:
    PropsMenuToolBar(Layout bar);

    void clear();

    void addButton(ButtonKey::Enum key, const std::function<void()>& callback);

private:
    Layout m_bar;
    std::unordered_map<ButtonKey::Enum, Button> m_buttons;
};

} }
