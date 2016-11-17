/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SharedContext.h"

namespace gamebase { namespace editor {

SharedContext::SharedContext(
    TreeView& treeView,
    Selector propertiesMenu,
    DesignModel& model)
    : treeView(treeView)
    , propertiesMenu(propertiesMenu)
    , model(model)
{}

void SharedContext::select(int id)
{
    propertiesMenu.select(id);
    toolBar->clear();
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        const auto& buttonCallbacks = it->second.callbacks;
        for (auto itButton = buttonCallbacks.begin(); itButton != buttonCallbacks.end(); ++itButton)
            toolBar->addButton(itButton->first, itButton->second);
    }
    propertiesMenuArea.update();
}

void SharedContext::onSelection()
{
    select(switchsGroup.selected());
}

} }
