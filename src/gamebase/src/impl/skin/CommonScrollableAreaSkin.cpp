/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/CommonScrollableAreaSkin.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

CommonScrollableAreaSkin::CommonScrollableAreaSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeBox>& areaBox)
    : m_box(box)
    , m_areaBox(areaBox)
{}

void CommonScrollableAreaSkin::setScrollBarSkin(
    const std::shared_ptr<ScrollBarSkin>& skin,
    Direction::Enum direction,
    const std::shared_ptr<IRelativeOffset>& position)
{
    auto newPosition = position;
    if (skin) {
        if (!newPosition) {
            RelativeValue horOffset(RelType::Pixels, 0.0f);
            RelativeValue vertOffset(RelType::Ratio, -1.0f);
            if (direction == Direction::Vertical) {
                horOffset = RelativeValue();
                vertOffset = RelativeValue(RelType::Pixels, 0.0f);
            }
            newPosition = std::make_shared<AligningOffset>(
                direction == Direction::Horizontal ? HorAlign::Center : HorAlign::Left,
                direction == Direction::Horizontal ? VertAlign::Top : VertAlign::Center,
                horOffset, vertOffset);
        }
    }
    m_scrollBarSkins[direction] = skin;
    m_scrollBarPositions[direction] = newPosition;
}

std::shared_ptr<ScrollBar> CommonScrollableAreaSkin::createScrollBar(
    const std::shared_ptr<FloatValue>& controlledValue,
    Direction::Enum direction) const
{
    if (!m_scrollBarSkins[direction])
        return nullptr;
    auto scrollBar = std::make_shared<ScrollBar>(
        m_scrollBarSkins[direction], m_scrollBarPositions[direction]);
    scrollBar->setControlledValue(controlledValue);
    return scrollBar;
}

void CommonScrollableAreaSkin::setSize(float width, float height)
{
    if (!m_areaWithoutScrollBarsBox)
        return;
    m_curAreaBox = m_areaWithoutScrollBarsBox->get();
    if (width > m_curAreaBox.width()) {
        m_curAreaBox.bottomLeft.y = m_areaBox->get().bottomLeft.y;
        m_curAreaBox.topRight.y = m_areaBox->get().topRight.y;
    }
    if (height > m_curAreaBox.height()) {
        m_curAreaBox.bottomLeft.x = m_areaBox->get().bottomLeft.x;
        m_curAreaBox.topRight.x = m_areaBox->get().topRight.x;
    }
    if (width > m_curAreaBox.width()) {
        m_curAreaBox.bottomLeft.y = m_areaBox->get().bottomLeft.y;
        m_curAreaBox.topRight.y = m_areaBox->get().topRight.y;
    }
}

void CommonScrollableAreaSkin::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
}

void CommonScrollableAreaSkin::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    m_areaBox->setParentBox(box());
    if (m_areaWithoutScrollBarsBox) {
        m_areaWithoutScrollBarsBox->setParentBox(box());
        m_curAreaBox = m_areaWithoutScrollBarsBox->get();
    } else {
        m_curAreaBox = m_areaBox->get();
    }
    m_skinElements.setBox(box());
}

void CommonScrollableAreaSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_skinElements);
    if (m_scrollBarSkins[Direction::Horizontal])
        builder->registerObject(m_scrollBarSkins[Direction::Horizontal].get());
    if (m_scrollBarSkins[Direction::Vertical])
        builder->registerObject(m_scrollBarSkins[Direction::Vertical].get());
}

void CommonScrollableAreaSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "areaBox" << m_areaBox
        << "areaWithoutScrollBarsBox" << m_areaWithoutScrollBarsBox
        << "horScrollBarSkin" << m_scrollBarSkins[Direction::Horizontal]
        << "horScrollBarPosition" << m_scrollBarPositions[Direction::Horizontal]
        << "vertScrollBarSkin" << m_scrollBarSkins[Direction::Vertical]
        << "vertScrollBarPosition" << m_scrollBarPositions[Direction::Vertical]
        << "elements" << m_skinElements.objects();
}

std::unique_ptr<IObject> deserializeCommonScrollableAreaSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, areaBox);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, areaWithoutScrollBarsBox);
    DESERIALIZE(std::shared_ptr<ScrollBarSkin>, horScrollBarSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, horScrollBarPosition);
    DESERIALIZE(std::shared_ptr<ScrollBarSkin>, vertScrollBarSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, vertScrollBarPosition);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    std::unique_ptr<CommonScrollableAreaSkin> result(
        new CommonScrollableAreaSkin(box, areaBox));
    result->setAreaWithoutScrollBarsBox(areaWithoutScrollBarsBox);
    result->setScrollBarSkin(horScrollBarSkin, Direction::Horizontal, horScrollBarPosition);
    result->setScrollBarSkin(vertScrollBarSkin, Direction::Vertical, vertScrollBarPosition);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return std::move(result);
}

REGISTER_CLASS(CommonScrollableAreaSkin);

} }
