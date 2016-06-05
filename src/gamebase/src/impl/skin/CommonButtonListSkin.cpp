#include <stdafx.h>
#include <gamebase/impl/skin/impl/CommonButtonListSkin.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

CommonButtonListSkin::CommonButtonListSkin(
    const std::shared_ptr<IRelativeBox>& listBox,
    const std::shared_ptr<IRelativeBox>& extentBox,
    Direction::Enum direction)
    : m_listBox(listBox)
    , m_extentBox(extentBox)
    , m_direction(direction)
    , m_padding(0.0f)
    , m_adjustSize(false)
{
    if (!m_extentBox)
        m_extentBox = std::make_shared<OffsettedBox>();
}

void CommonButtonListSkin::setScrollBarSkin(
    const std::shared_ptr<ScrollBarSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    auto newPosition = position;
    if (skin) {
        if (!newPosition) {
            RelativeValue horOffset(RelType::Pixels, 0.0f);
            RelativeValue vertOffset(RelType::Ratio, -1.0f);
            if (m_direction == Direction::Vertical) {
                horOffset = RelativeValue();
                vertOffset = RelativeValue(RelType::Pixels, 0.0f);
            }
            newPosition = std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Center : HorAlign::Left,
                m_direction == Direction::Horizontal ? VertAlign::Top : VertAlign::Center,
                horOffset, vertOffset);
        }
    }
    m_scrollBarSkin = skin;
    m_scrollBarPosition = newPosition;
}

std::shared_ptr<ScrollBar> CommonButtonListSkin::createScrollBar(
    const std::shared_ptr<FloatValue>& controlledValue) const
{
    if (!m_scrollBarSkin)
        return nullptr;
    auto scrollBar = std::make_shared<ScrollBar>(
        m_scrollBarSkin, m_scrollBarPosition);
    scrollBar->setControlledValue(controlledValue);
    return scrollBar;
}

std::shared_ptr<IRelativeOffset> CommonButtonListSkin::createButtonOffset(size_t index) const
{
    float offset = index == 0 ? 0.0f : m_padding;
    float horOffset = offset;
    float vertOffset = 0.0f;
    if (m_direction == Direction::Vertical) {
        vertOffset = -offset;
        horOffset = 0.0f;
    }
    return std::make_shared<AligningOffset>(HorAlign::Left, VertAlign::Top,
        RelativeValue(RelType::Pixels, horOffset),
        RelativeValue(RelType::Pixels, vertOffset));
}

void CommonButtonListSkin::setSize(float size)
{
    float maxSize = m_direction == Direction::Horizontal
        ? m_listBox->get().width() : m_listBox->get().height();
    if (size < maxSize && m_listWithoutScrollBarsBox
        && (!m_scrollBarSkin || !m_scrollBarSkin->alwaysShow())) {
        m_curListBox = m_listWithoutScrollBarsBox->get();
    } else {
        m_curListBox = m_listBox->get();
    }

    if (m_adjustSize) {
        if (size < maxSize) {
            if (m_direction == Direction::Horizontal)
                m_curListBox.topRight.x = m_curListBox.bottomLeft.x + size;
            else
                m_curListBox.bottomLeft.y = m_curListBox.topRight.y - size;
        }
    }
    m_extentBox->setParentBox(m_curListBox);
    m_skinElements.setBox(box());
}

void CommonButtonListSkin::setBox(const BoundingBox& allowedBox)
{
    m_listBox->setParentBox(allowedBox);
    if (m_listWithoutScrollBarsBox) {
        m_listWithoutScrollBarsBox->setParentBox(allowedBox);
        m_curListBox = m_listWithoutScrollBarsBox->get();
    } else {
        m_curListBox = m_listBox->get();
    }
    m_extentBox->setParentBox(m_curListBox);
    m_skinElements.setBox(box());
}

void CommonButtonListSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_skinElements);
    if (m_scrollBarSkin)
        builder->registerObject(m_scrollBarSkin.get());
}

void CommonButtonListSkin::serialize(Serializer& s) const
{
    s << "listBox" << m_listBox << "extentBox" << m_extentBox
        << "direction" << m_direction << "listWithoutScrollBarsBox" << m_listWithoutScrollBarsBox
        << "padding" << m_padding << "adjustSize" << m_adjustSize
        << "scrollBarSkin" << m_scrollBarSkin << "scrollBarPosition" << m_scrollBarPosition
        << "elements" << m_skinElements.objects();
}

std::unique_ptr<IObject> deserializeCommonButtonListSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, listBox);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, extentBox);
    DESERIALIZE(Direction::Enum, direction);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, listWithoutScrollBarsBox);
    DESERIALIZE(float, padding);
    DESERIALIZE(bool, adjustSize);
    DESERIALIZE(std::shared_ptr<ScrollBarSkin>, scrollBarSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, scrollBarPosition);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    std::unique_ptr<CommonButtonListSkin> result(
        new CommonButtonListSkin(listBox, extentBox, direction));
    result->setListWithoutScrollBarsBox(listWithoutScrollBarsBox);
    result->setPadding(padding);
    result->setAdjustSize(adjustSize);
    result->setScrollBarSkin(scrollBarSkin, scrollBarPosition);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return std::move(result);
}

REGISTER_CLASS(CommonButtonListSkin);

} }
