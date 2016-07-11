#pragma once

#include <gamebase/Gamebase.h>

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
