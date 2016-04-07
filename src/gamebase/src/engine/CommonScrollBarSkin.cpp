#include <stdafx.h>
#include <gamebase/engine/CommonScrollBarSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

CommonScrollBarSkin::CommonScrollBarSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeBox>& dragBox,
    Direction::Enum direction)
    : m_box(box)
    , m_dragBox(dragBox ? dragBox : std::make_shared<OffsettedBox>())
    , m_direction(direction)
    , m_alwaysShow(false)
    , m_step(false)
{}

void CommonScrollBarSkin::setDecButtonSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_decButtonSkin = skin;
    if (position) {
        m_decButtonPosition = position;
    } else {
        if (skin)
            m_decButtonPosition = std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Left : HorAlign::Center,
                m_direction == Direction::Horizontal ? VertAlign::Center : VertAlign::Bottom);
        else
            m_decButtonPosition.reset();
    }
}

void CommonScrollBarSkin::setIncButtonSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_incButtonSkin = skin;
    if (position) {
        m_incButtonPosition = position;
    } else {
        if (skin)
            m_incButtonPosition = std::make_shared<AligningOffset>(
                m_direction == Direction::Horizontal ? HorAlign::Right : HorAlign::Center,
                m_direction == Direction::Horizontal ? VertAlign::Center : VertAlign::Top);
        else
            m_incButtonPosition.reset();
    }
}

std::shared_ptr<Button> CommonScrollBarSkin::createDecButton() const
{
    return m_decButtonSkin
        ? std::make_shared<Button>(m_decButtonSkin, m_decButtonPosition)
        : nullptr;
}

std::shared_ptr<Button> CommonScrollBarSkin::createIncButton() const
{
    return m_incButtonSkin
        ? std::make_shared<Button>(m_incButtonSkin, m_incButtonPosition)
        : nullptr;
}

std::shared_ptr<ScrollDragBar> CommonScrollBarSkin::createDragBar(
    const std::shared_ptr<FixedOffset>& position) const
{
    return m_dragBarSkin
        ? std::make_shared<ScrollDragBar>(m_dragBarSkin, position)
        : nullptr;
}

void CommonScrollBarSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_skinElements);
    if (m_incButtonSkin)
        builder->registerObject(m_incButtonSkin.get());
    if (m_decButtonSkin)
        builder->registerObject(m_decButtonSkin.get());
    if (m_dragBarSkin)
        builder->registerObject(m_dragBarSkin.get());
}

void CommonScrollBarSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "dragBox" << m_dragBox << "direction" << m_direction
        << "decButtonSkin" << m_decButtonSkin << "decButtonPosition" << m_decButtonPosition
        << "incButtonSkin" << m_incButtonSkin << "incButtonPosition" << m_incButtonPosition
        << "dragBarSkin" << m_dragBarSkin << "alwaysShow" << m_alwaysShow
        << "step" << m_step << "elements" << m_skinElements.objects();
}

std::unique_ptr<IObject> deserializeCommonScrollBarSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, dragBox);
    DESERIALIZE(Direction::Enum, direction);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, decButtonSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, decButtonPosition);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, incButtonSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, incButtonPosition);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, dragBarSkin);
    DESERIALIZE(bool, alwaysShow);
    DESERIALIZE(float, step);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    std::unique_ptr<CommonScrollBarSkin> result(
        new CommonScrollBarSkin(box, dragBox, direction));
    result->setDecButtonSkin(decButtonSkin, decButtonPosition);
    result->setIncButtonSkin(incButtonSkin, incButtonPosition);
    result->setDragBarSkin(dragBarSkin);
    result->setAlwaysShow(alwaysShow);
    result->setStep(step);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return std::move(result);
}

REGISTER_CLASS(CommonScrollBarSkin);

}
