/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {

struct ButtonKey {
    enum Enum {
        Add,
        AddFromFile,
        Remove,
        Save,
        ReplaceFromFile,
        Down,
        Up,
        Copy,
        Paste,
        AddFromClipboard
    };
};

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
