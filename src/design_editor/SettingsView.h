/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/Layout.h>

namespace gamebase { namespace editor {

class SettingsView {
public:
    SettingsView() {}

    void init(Layout layout);

private:
    void apply();

    Layout m_layout;
};

} }
