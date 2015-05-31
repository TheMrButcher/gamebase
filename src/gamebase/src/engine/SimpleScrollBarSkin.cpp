#include <stdafx.h>
#include <gamebase/engine/SimpleScrollBarSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

void SimpleScrollBarSkin::setDecButtonSkin(
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

void SimpleScrollBarSkin::setIncButtonSkin(
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

std::shared_ptr<Button> SimpleScrollBarSkin::createDecButton() const
{
    return m_decButtonSkin
        ? std::make_shared<Button>(m_decButtonPosition, m_decButtonSkin)
        : nullptr;
}

std::shared_ptr<Button> SimpleScrollBarSkin::createIncButton() const
{
    return m_incButtonSkin
        ? std::make_shared<Button>(m_incButtonPosition, m_incButtonSkin)
        : nullptr;
}

std::shared_ptr<ScrollDragBar> SimpleScrollBarSkin::createDragBar(
    const std::shared_ptr<FixedOffset>& position) const
{
    return m_dragBarSkin
        ? std::make_shared<ScrollDragBar>(position, m_dragBarSkin)
        : nullptr;
}

void SimpleScrollBarSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("elements", &m_skinElements);
    if (m_incButtonSkin)
        builder->registerObject("incButton", m_incButtonSkin.get());
    if (m_decButtonSkin)
        builder->registerObject("decButton", m_decButtonSkin.get());
    if (m_dragBarSkin)
        builder->registerObject("dragBar", m_dragBarSkin.get());
}

void SimpleScrollBarSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "dragBox" << m_dragBox << "direction" << m_direction
        << "decButtonSkin" << m_decButtonSkin << "decButtonPosition" << m_decButtonPosition
        << "incButtonSkin" << m_incButtonSkin << "incButtonPosition" << m_incButtonPosition
        << "dragBarSkin" << m_dragBarSkin << "alwaysShow" << m_alwaysShow
        << "elements" << m_skinElements.objects();
}

std::unique_ptr<IObject> deserializeSimpleScrollBarSkin(Deserializer& deserializer)
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
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    std::unique_ptr<SimpleScrollBarSkin> result(
        new SimpleScrollBarSkin(box, dragBox, direction));
    result->setDecButtonSkin(decButtonSkin, decButtonPosition);
    result->setIncButtonSkin(incButtonSkin, incButtonPosition);
    result->setDragBarSkin(dragBarSkin);
    result->setAlwaysShow(alwaysShow);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return std::move(result);
}

REGISTER_CLASS(SimpleScrollBarSkin);

}
