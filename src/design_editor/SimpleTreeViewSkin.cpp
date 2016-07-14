/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "SimpleTreeViewSkin.h"
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

SimpleTreeViewSkin::SimpleTreeViewSkin(
    const std::shared_ptr<impl::IRelativeBox>& box)
    : m_box(box)
    , m_treeBox(std::make_shared<impl::RelativeBox>(
        impl::RelativeValue(impl::RelType::ValuePlusPixels, 20.0f), impl::RelativeValue(),
        std::make_shared<impl::FixedOffset>(-10.0f, 0.0f)))
{}

std::shared_ptr<impl::ScrollableArea> SimpleTreeViewSkin::createTreeArea() const
{
    return impl::deserialize<impl::ScrollableArea>("ui\\ScrollableArea.json");
}

std::shared_ptr<impl::ToggleButton> SimpleTreeViewSkin::createOpenButton() const
{
    auto skin = impl::deserialize<impl::ButtonSkin>("ui\\ArrowButton.json");
    auto button = std::make_shared<impl::ToggleButton>(
        skin, std::make_shared<impl::AligningOffset>(impl::HorAlign::Center, impl::VertAlign::Center));
    button->setUnpressOnFocusLost(false);
    return button;
}

std::shared_ptr<impl::IRelativeBox> SimpleTreeViewSkin::createSubtreeBox() const
{
    return std::make_shared<impl::OffsettedBox>(std::make_shared<impl::FixedOffset>(20.0f, -2.0f));
}

std::shared_ptr<impl::IRelativeOffset> SimpleTreeViewSkin::createOffset() const
{
    return std::make_shared<impl::AligningOffset>(
        impl::HorAlign::Left, impl::VertAlign::Top,
        impl::RelativeValue(impl::RelType::Pixels, 22.0f), impl::RelativeValue(impl::RelType::Pixels, 0.0f));
}
    
} }
