/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/pubhelp/FromImpl.h>
#include <gamebase/impl/adapt/LinearLayoutAdapter.h>
#include <gamebase/impl/adapt/CanvasLayoutAdapter.h>
#include <gamebase/impl/adapt/ScrollableLayoutAdapter.h>
#include <gamebase/impl/adapt/SelectingLayoutAdapter.h>
#include <gamebase/impl/adapt/OffsettedDrawObjAdapter.h>
#include <gamebase/impl/adapt/ObjectConstructDrawObjAdapter.h>
#include <gamebase/impl/adapt/GameObjAdapter.h>
#include <gamebase/ui/Layout.h>
#include <gamebase/drawobj/DrawObj.h>
#include <gamebase/gameobj/GameObj.h>

namespace gamebase { namespace impl {

std::shared_ptr<ILayoutAdapter> FromImpl<Layout>::cast(
    const SmartPointer<IObject>& obj)
{
    auto canvasLayout = SmartPointer<CanvasLayout>(obj);
    if (canvasLayout)
        return std::make_shared<CanvasLayoutAdapter>(canvasLayout);
    auto linearLayout = SmartPointer<LinearLayout>(obj);
    if (linearLayout)
        return std::make_shared<LinearLayoutAdapter>(linearLayout);
    auto selectingWidget = SmartPointer<SelectingWidget>(obj);
    if (selectingWidget)
        return std::make_shared<SelectingLayoutAdapter>(selectingWidget);
    auto scrollableArea = SmartPointer<ScrollableArea>(obj);
    if (scrollableArea)
        return std::make_shared<ScrollableLayoutAdapter>(scrollableArea);
    return nullptr;
}

std::shared_ptr<IDrawObjAdapter> FromImpl<DrawObj>::cast(
    const SmartPointer<IObject>& obj)
{
    auto offsettedPos = SmartPointer<OffsettedPosition>(obj);
    if (offsettedPos) {
        auto drawable = dynamic_cast<IDrawable*>(obj.get());
        if (!drawable)
            return nullptr;
        return std::make_shared<OffsettedDrawObjAdapter>(offsettedPos, drawable);
    }
    auto objectConstruct = SmartPointer<InactiveObjectConstruct>(obj);
    if (objectConstruct)
        return std::make_shared<ObjectConstructDrawObjAdapter>(objectConstruct);
    return nullptr;
}

std::shared_ptr<GameObjAdapter> FromImpl<GameObj>::cast(
    const SmartPointer<IObject>& obj)
{
    auto objectConstruct = SmartPointer<InactiveObjectConstruct>(obj);
    if (objectConstruct)
        return std::make_shared<GameObjAdapter>(objectConstruct);
    return nullptr;
}

} }
