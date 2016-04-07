#include "SimpleTreeViewSkin.h"
#include <gamebase/engine/RelativeBox.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/JsonDeserializer.h>

namespace gamebase { namespace editor {

SimpleTreeViewSkin::SimpleTreeViewSkin(
    const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
    , m_treeBox(std::make_shared<RelativeBox>(
        RelativeValue(RelType::ValuePlusPixels, 20.0f), RelativeValue(),
        std::make_shared<FixedOffset>(-10.0f, 0.0f)))
{}

std::shared_ptr<ScrollableArea> SimpleTreeViewSkin::createTreeArea() const
{
    auto skin = loadObj<ScrollableAreaSkin>("ui\\ScrollableAreaSkin.json");
    return std::make_shared<ScrollableArea>(skin);
}

std::shared_ptr<ToggleButton> SimpleTreeViewSkin::createOpenButton() const
{
    auto skin = loadObj<ButtonSkin>("ui\\ArrowButton.json");
    auto button = std::make_shared<ToggleButton>(
        skin, std::make_shared<AligningOffset>(HorAlign::Center, VertAlign::Center));
    button->setUnpressOnFocusLost(false);
    return button;
}

std::shared_ptr<IRelativeBox> SimpleTreeViewSkin::createSubtreeBox() const
{
    return std::make_shared<OffsettedBox>(std::make_shared<FixedOffset>(20.0f, -2.0f));
}

std::shared_ptr<IRelativeOffset> SimpleTreeViewSkin::createOffset() const
{
    return std::make_shared<AligningOffset>(
        HorAlign::Left, VertAlign::Top,
        RelativeValue(RelType::Pixels, 22.0f), RelativeValue(RelType::Pixels, 0.0f));
}
    
} }
