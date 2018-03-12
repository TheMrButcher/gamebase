/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Properties.h"
#include <gamebase/serial/LoadObj.h>
#include <gamebase/drawobj/DrawObj.h>

namespace gamebase { namespace editor {
Properties::Properties()
    : id(-1)
    , type(nullptr)
    , presentationFromParent(nullptr)
    , keyPresentationFromParent(nullptr)
    , isInline(false)
    , isHiddenLevel(false)
{}

void Properties::attach(Layout layout)
{
    if (isHiddenLevel)
        return;

    size_t visiblePropsNum = 0;
    if (isInline) {
        auto propsLayout = loadObj<Layout>("ui\\InlinedObjLayout.json");
        if (m_labelUpdater)
            m_labelUpdater(propsLayout.child<Label>("label"));
        auto innerLayout = propsLayout.child<Layout>("inner");
        visiblePropsNum = addSelfProperties(innerLayout);
        layout.add(propsLayout);
    } else {
        visiblePropsNum = addSelfProperties(layout);
    }

    size_t visibleInlinedPropsNum = 0;
    for (const auto& props : inlined) {
        if (!isInline && !props->isHiddenLevel) {
            if (visiblePropsNum != 0 || visibleInlinedPropsNum != 0)
                layout.add(loadObj<DrawObj>("ui\\HorLine.json"));
        }
        props->attach(layout);
        if (!props->isHiddenLevel)
            visibleInlinedPropsNum++;
    }
}

void Properties::sync()
{
    for (const auto& prop : list)
        prop->sync();
    for (const auto& props : inlined)
        props->sync();
}

void Properties::detach()
{
    for (const auto& prop : list)
        prop->detach();
    for (const auto& props : inlined)
        props->detach();
}

size_t Properties::addSelfProperties(Layout layout)
{
    size_t visiblePropsNum = 0;
    for (const auto& prop : list) {
        if (!prop->isHidden())
            visiblePropsNum++;
        prop->attach(layout, labelUpdater());
    }
    return visiblePropsNum;
}

} }
