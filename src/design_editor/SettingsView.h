#pragma once

namespace gamebase {

class CanvasLayout;

namespace editor {

class SettingsView {
public:
    SettingsView() {}

    void init(CanvasLayout* layout);

private:
    void apply();

    CanvasLayout* m_layout;
};

} }
