#include "NewObjDialog.h"
#include "Presentation.h"
#include <gamebase/engine/Panel.h>
#include <gamebase/engine/RadioButton.h>
#include <gamebase/engine/SelectingWidget.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/ScrollableArea.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/CommonScrollableAreaSkin.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <unordered_map>

namespace gamebase { namespace editor {

void NewObjDialog::init(
    Panel* panel,
    const std::function<void(const std::string&)>& okCallback)
{
    m_panel = panel;
    m_ok = panel->getChild<Button>("#ok");
    m_cancel = panel->getChild<Button>("#cancel");
    m_panel->setVisible(false);
    m_ok->setCallback(std::bind(&NewObjDialog::processResult, this, okCallback));
    m_cancel->setCallback(std::bind(&Panel::close, m_panel));

    m_mainGroup = std::make_shared<RadioButtonGroup>();

    {
        auto selector = std::make_shared<SelectingWidget>(
            std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
        m_selector = selector.get();
        m_panel->getChild<LinearLayout>("#main")->addObject(selector);
        m_mainGroup->setCallback(std::bind(&SelectingWidget::select, m_selector, std::placeholders::_1));
    }

    addGroup("#animation", "IAnimation");
    addGroup("#layout", "Layout");
    addGroup("#construct", "InactiveObjectConstruct");
    addGroup("#primitive", "SimpleElement");
    addGroup("#ui", "UIElement");

    auto presentation = presentationForDesignView();
    auto allObjects = presentation->derivedTypesByBaseTypeName("IObject");
    std::map<std::string, std::string> types;
    for (auto it = allObjects.begin(); it != allObjects.end(); ++it) {
        if (!SerializableRegister::instance().isRegistered((*it)->name))
            continue;
        types[(*it)->nameInUI] = (*it)->name;
    }
    for (auto it = types.begin(); it != types.end(); ++it)
        addClass(it->second, it->first);
}

void NewObjDialog::addGroup(const std::string& buttonName, const std::string& groupName)
{
    auto button = m_panel->getChild<RadioButton>(buttonName);
    button->setGroup(m_mainGroup);
    int id = button->indexInGroup();
    m_nameToGroupID[groupName] = id;

    auto& group = m_groups[id];
    group.radioButtons = std::make_shared<RadioButtonGroup>();
    auto area = std::make_shared<ScrollableArea>(
        deserialize<CommonScrollableAreaSkin>("ui\\ScrollableAreaSkin.json"));
    auto layout = deserialize<LinearLayout>("ui\\VertCenterLayout.json");
    area->objects().addObject(layout);
    group.layout = layout.get();
    m_selector->addObject(id, area);
}

void NewObjDialog::addClass(const std::string& id, const std::string& uiName)
{
    auto presentation = presentationForDesignView();
    auto baseTypes = presentation->baseTypesByTypeName(id);
    for (auto it = baseTypes.begin(); it != baseTypes.end(); ++it) {
        auto groupIt = m_nameToGroupID.find(*it);
        if (groupIt != m_nameToGroupID.end()) {
            auto& group = m_groups[groupIt->second];
            
            auto skin = deserialize<CheckBoxSkin>("ui\\NewObjSwitchSkin.json");
            auto label = id.find_first_of("<>") != std::string::npos
                ? uiName
                : uiName + " (" + id + ")";
            skin->getChild<StaticLabel>("#label")->setText(label);
            auto button = std::make_shared<RadioButton>(skin);
            button->setGroup(group.radioButtons);
            group.layout->addObject(button);
            group.variants[button->indexInGroup()] = presentation->pathToPattern(id);
        }
    }
}

void NewObjDialog::processResult(const std::function<void(const std::string&)>& callback)
{
    if (!m_mainGroup->isAnySelected())
        return;
    auto& group = m_groups[m_mainGroup->selected()];
    if (!group.radioButtons->isAnySelected())
        return;
    auto id = group.radioButtons->selected();
    auto pathToPattern = group.variants[id];
    m_panel->setVisible(false);
    if (callback)
        callback(pathToPattern);
}

} }
