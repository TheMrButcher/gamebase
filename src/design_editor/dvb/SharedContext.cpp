/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SharedContext.h"
#include <gamebase/serial/LoadObj.h>
#include <gamebase/tools/MakeRaw.h>

namespace gamebase { namespace editor {

SharedContext::SharedContext(
    TreeView& treeView,
    Selector propertiesMenu,
    DesignModel& model)
    : treeView(treeView)
    , propertiesMenu(propertiesMenu)
    , model(model)
    , currentNodeID(-1)
{}

void SharedContext::select(int id)
{
    sync();

    toolBar->clear();
    propertiesMenu.select(-1);

    auto itOld = nodes.find(currentNodeID);
    if (itOld != nodes.end() && itOld->second.props) {
        itOld->second.props->detach();
        propertiesMenu.remove(currentNodeID);
    }

    currentNodeID = id;
    auto it = nodes.find(id);
    if (it == nodes.end() || !it->second.props) {
        propertiesMenuArea.update();
        return;
    }
    if (!propertiesMenu.has(id)) {
        Layout layout = loadObj<Layout>("ui\\PropertiesLayout.json");
        const auto& props = nodes[id].props;
        nodes[id].props->attach(makeRaw(layout));
        propertiesMenu.insert(props->id, layout);
    }
    propertiesMenu.select(id);

    for (const auto& keyAndButton : it->second.callbacks)
        toolBar->addButton(keyAndButton.first, keyAndButton.second);
    propertiesMenuArea.update();
}

void SharedContext::onSelection()
{
    select(switchsGroup.selected());
}

void SharedContext::sync()
{
    auto it = nodes.find(currentNodeID);
    if (it != nodes.end() && it->second.props)
        it->second.props->sync();
}

} }
