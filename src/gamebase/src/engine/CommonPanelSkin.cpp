#include <stdafx.h>
#include <gamebase/engine/CommonPanelSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/engine/OffsettedBox.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

CommonPanelSkin::CommonPanelSkin(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeBox>& panelBox)
    : m_box(box)
    , m_panelBox(panelBox)
{
    if (!m_panelBox)
        m_panelBox = std::make_shared<OffsettedBox>();
}

void CommonPanelSkin::setCloseButtonSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_closeButtonSkin = skin;
    m_closeButtonPosition = skin
        ? position
            ? position
            : std::make_shared<AligningOffset>(
                HorAlign::Right, VertAlign::Top)
        : std::shared_ptr<IRelativeOffset>();
}

void CommonPanelSkin::setDragBarSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_dragBarSkin = skin;
    m_dragBarPosition = skin
        ? position
            ? position
            : std::make_shared<AligningOffset>(
                HorAlign::Left, VertAlign::Top)
        : std::shared_ptr<IRelativeOffset>();
}

std::shared_ptr<Button> CommonPanelSkin::createCloseButton() const
{
    if (!m_closeButtonSkin)
        return nullptr;
    return std::make_shared<Button>(m_closeButtonPosition, m_closeButtonSkin);
}

std::shared_ptr<ScrollDragBar> CommonPanelSkin::createDragBar() const
{
    if (!m_dragBarSkin)
        return nullptr;
    return std::make_shared<ScrollDragBar>(m_dragBarPosition, m_dragBarSkin);
}

void CommonPanelSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("elements", &m_skinElements);
    if (m_closeButtonSkin)
        builder->registerObject("closeButton", m_closeButtonSkin.get());
    if (m_dragBarSkin)
        builder->registerObject("dragBar", m_dragBarSkin.get());
}

void CommonPanelSkin::serialize(Serializer& s) const
{
    s << "box" << m_box << "panelBox" << m_panelBox
        << "closeButtonSkin" << m_closeButtonSkin << "closeButtonPosition" << m_closeButtonPosition
        << "dragBarSkin" << m_dragBarSkin << "dragBarPosition" << m_dragBarPosition
        << "elements" << m_skinElements.objects() << "isLimitedByBox" << m_isLimitedByBox;
}

std::unique_ptr<IObject> deserializeCommonPanelSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, panelBox);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, closeButtonSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, closeButtonPosition);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, dragBarSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, dragBarPosition);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);
    DESERIALIZE(bool, isLimitedByBox);

    std::unique_ptr<CommonPanelSkin> result(new CommonPanelSkin(box, panelBox));
    result->setCloseButtonSkin(closeButtonSkin, closeButtonPosition);
    result->setDragBarSkin(dragBarSkin, dragBarPosition);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    result->setIsLimitedByBox(isLimitedByBox);
    return std::move(result);
}

REGISTER_CLASS(CommonPanelSkin);

}
