#include "NewObjDialog.h"
#include "Presentation.h"
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/ui/ScrollableArea.h>
#include <gamebase/impl/skin/impl/CommonScrollableAreaSkin.h>

namespace gamebase { namespace editor {

void NewObjDialog::init(
    Panel panel,
    const std::function<void(const std::string&)>& okCallback)
{
    m_panel = panel;
    m_ok = panel.child<Button>("ok");
    m_cancel = panel.child<Button>("cancel");
    m_panel.hide();
    m_ok.setCallback(std::bind(&NewObjDialog::processResult, this, okCallback));
    m_cancel.setCallback(std::bind(&Panel::hide, m_panel));

    m_selector = Selector(impl::SmartPointer<impl::SelectingWidget>(std::make_shared<impl::SelectingWidget>(
        std::make_shared<impl::RelativeBox>(impl::RelativeValue(), impl::RelativeValue()))));
    m_panel.child<Layout>("main").add(m_selector);
    m_mainGroup.setCallback(std::bind(&NewObjDialog::selectGroup, this));

    addGroup("animation", "IAnimation");
    addGroup("layout", "Layout");
    addGroup("construct", "InactiveObjectConstruct");
    addGroup("primitive", "SimpleElement");
    addGroup("ui", "UIElement");
    addGroup("text", "TextBank");

    auto presentation = presentationForDesignView();
    auto allObjects = presentation->derivedTypesByBaseTypeName("IObject");
    std::map<std::string, std::string> types;
    for (auto it = allObjects.begin(); it != allObjects.end(); ++it) {
        if (!impl::SerializableRegister::instance().isRegistered((*it)->name))
            continue;
        types[(*it)->nameInUI] = (*it)->name;
    }
    for (auto it = types.begin(); it != types.end(); ++it)
        addClass(it->second, it->first);
}

void NewObjDialog::addGroup(const std::string& buttonName, const std::string& groupName)
{
    auto button = m_panel.child<RadioButton>(buttonName);
    int id = m_mainGroup.add(button);
    m_nameToGroupID[groupName] = id;

    auto& group = m_groups[id];
    auto area = std::make_shared<impl::ScrollableArea>(
        impl::deserialize<impl::CommonScrollableAreaSkin>("ui\\ScrollableAreaSkin.json"));
    group.layout = loadObj<Layout>("ui\\VertCenterLayout.json");
    area->objects().addObject(impl::unwrapShared(group.layout));
    m_selector.getImpl()->insertObject(id, area);
}

void NewObjDialog::addClass(const std::string& id, const std::string& uiName)
{
    auto presentation = presentationForDesignView();
    auto baseTypes = presentation->baseTypesByTypeName(id);
    for (auto it = baseTypes.begin(); it != baseTypes.end(); ++it) {
        auto groupIt = m_nameToGroupID.find(*it);
        if (groupIt != m_nameToGroupID.end()) {
            auto& group = m_groups[groupIt->second];
            
            auto skin = impl::deserialize<impl::CheckBoxSkin>("ui\\NewObjSwitchSkin.json");
            auto label = id.find_first_of("<>") != std::string::npos
                ? uiName
                : uiName + " (" + id + ")";
            skin->getChild<impl::StaticLabel>("label")->setText(label);
            RadioButton button(impl::SmartPointer<impl::RadioButton>(
                std::make_shared<impl::RadioButton>(skin)));
            group.radioButtons.add(button);
            group.layout.add(button);
            group.variants[button.id()] = presentation->pathToPattern(id);
        }
    }
}

void NewObjDialog::processResult(const std::function<void(const std::string&)>& callback)
{
    if (m_mainGroup.selected() == -1)
        return;
    auto& group = m_groups[m_mainGroup.selected()];
    if (group.radioButtons.selected() == -1)
        return;
    auto id = group.radioButtons.selected();
    auto pathToPattern = group.variants[id];
    m_panel.hide();
    if (callback)
        callback(pathToPattern);
}

} }
