#pragma once

#include <gamebase/engine/Panel.h>
#include <gamebase/engine/RadioButtonGroup.h>
#include <unordered_map>

namespace gamebase {

class LinearLayout;
class SelectingWidget;

namespace editor {

class NewObjDialog {
public:
    NewObjDialog() {}

    void init(
        Panel* panel,
        const std::function<void(const std::string&)>& okCallback);

    void run()
    {
        m_panel->setVisible(true);
    }

private:
    void addGroup(const std::string& buttonName, const std::string& groupName);
    void addClass(const std::string& id, const std::string& uiName);
    void processResult(const std::function<void(const std::string&)>& callback);

    Panel* m_panel;
    Button* m_ok;
    Button* m_cancel;

    std::shared_ptr<RadioButtonGroup> m_mainGroup;

    SelectingWidget* m_selector;

    std::unordered_map<std::string, int> m_nameToGroupID;

    struct Group {
        std::shared_ptr<RadioButtonGroup> radioButtons;
        LinearLayout* layout;
        std::map<int, std::string> variants;
    };
    std::map<int, Group> m_groups;
};

} }
