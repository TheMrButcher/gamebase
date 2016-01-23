#pragma once

#include <gamebase/engine/Panel.h>
#include <gamebase/engine/TextBox.h>

namespace gamebase {

class LinearLayout;
class SelectingWidget;

namespace editor {

class RunAnimationDialog {
public:
    RunAnimationDialog() {}

    void init(
        Panel* panel,
        const std::function<void(const std::string&, const std::string&)>& okCallback);

    void run()
    {
        m_panel->setVisible(true);
    }

private:
    Panel* m_panel;
    TextBox* m_
    Button* m_ok;
    Button* m_cancel;
};

} }
