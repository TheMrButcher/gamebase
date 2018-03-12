/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "NewObjDialog.h"
#include "Presentation.h"
#include "Settings.h"
#include <gamebase/impl/serial/SerializableRegister.h>
#include <gamebase/serial/LoadObj.h>
#include <gamebase/drawobj/Label.h>

namespace gamebase { namespace editor {

void NewObjDialog::init(
    Panel panel,
    const std::function<void(const std::string&)>& okCallback)
{
    m_panel = panel;
    m_ok = panel.child<Button>("ok");
    m_cancel = panel.child<Button>("cancel");
    m_panel.hide();
	m_ok.setCallback([this, okCallback]() { processResult(okCallback); });
	m_cancel.setCallback([this]() { m_panel.hide(); });

    m_selector = loadObj<Selector>("ui\\Selector.json");
    m_panel.child<Layout>("main").add(m_selector);
	m_mainGroup.setCallback([this]() { selectGroup(); });

    addGroup("layout", "Layout");
    if (settings::isComplexLayerMode())
        addGroup("game", "GameExt");
    else
        addGroup("game", "Game");
    addGroup("primitive", "SimpleElement");
    addGroup("ui", "UIElement");
    addGroup("other", "Additional");

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
    m_selector.update();

	m_mainGroup.select(m_nameToGroupID["Layout"]);
}

void NewObjDialog::addGroup(const std::string& buttonName, const std::string& groupName)
{
    auto button = m_panel.child<RadioButton>(buttonName);
    int id = m_mainGroup.add(button);
    m_nameToGroupID[groupName] = id;

    auto& group = m_groups[id];
    auto area = loadObj<Layout>("ui\\ScrollableArea.json");
    group.layout = loadObj<Layout>("ui\\VertCenterLayout.json");
    area.add(group.layout);
    m_selector.insert(id, area);
}

void NewObjDialog::addClass(const std::string& id, const std::string& uiName)
{
    auto presentation = presentationForDesignView();
    auto baseTypes = presentation->baseTypesByTypeName(id);
    for (auto it = baseTypes.begin(); it != baseTypes.end(); ++it) {
        auto groupIt = m_nameToGroupID.find(*it);
        if (groupIt != m_nameToGroupID.end()) {
            auto& group = m_groups[groupIt->second];
            
            auto label = id.find_first_of("<>") != std::string::npos
                ? uiName
                : uiName + " (" + id + ")";
            auto button = loadObj<RadioButton>("ui\\NewObjSwitchButton.json");
            button.child<Label>("label").setText(label);
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
