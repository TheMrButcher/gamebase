/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/ButtonKey.h>
#include <gamebase/Gamebase.h>

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
