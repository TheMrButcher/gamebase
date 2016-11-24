/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <dvb/Node.h>
#include <dvb/PropsMenuToolBar.h>
#include <TreeView.h>
#include <DesignModel.h>
#include <Presentation.h>
#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {
struct SharedContext {
    SharedContext(
        TreeView& treeView,
        Selector propertiesMenu,
        DesignModel& model);

    void select(int id);
    void onSelection();

    TreeView& treeView;
    Selector propertiesMenu;
    DesignModel& model;
    std::shared_ptr<Presentation> presentation;
    RadioGroup switchsGroup;
    std::shared_ptr<PropsMenuToolBar> toolBar;
    Layout propertiesMenuArea;
    std::unordered_map<int, Node> nodes;
};
} }