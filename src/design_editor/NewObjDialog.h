#pragma once

#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {

class NewObjDialog {
public:
    NewObjDialog() {}

    void init(
        Panel panel,
        const std::function<void(const std::string&)>& okCallback);

    void run() { m_panel.show(); }

private:
    void addGroup(const std::string& buttonName, const std::string& groupName);
    void addClass(const std::string& id, const std::string& uiName);
    void processResult(const std::function<void(const std::string&)>& callback);
    void selectGroup() { m_selector.select(m_mainGroup.selected()); }

    Panel m_panel;
    Button m_ok;
    Button m_cancel;
    RadioGroup m_mainGroup;
    Selector m_selector;

    struct Group {
        RadioGroup radioButtons;
        Layout layout;
        std::map<int, std::string> variants;
    };

    std::unordered_map<std::string, int> m_nameToGroupID;
    std::map<int, Group> m_groups;
};

} }
