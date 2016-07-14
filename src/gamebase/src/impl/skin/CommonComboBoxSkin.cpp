/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/CommonComboBoxSkin.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

CommonComboBoxSkin::CommonComboBoxSkin(
    const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
    , m_textBoxDisabled(false)
{}

void CommonComboBoxSkin::setOpenButtonSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_openButtonSkin = skin;
    m_openButtonPosition = position
        ? position
        : std::make_shared<AligningOffset>(
            HorAlign::Right, VertAlign::Center);
}

void CommonComboBoxSkin::setTextBoxSkin(
    const std::shared_ptr<TextBoxSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_textBoxSkin = skin;
    m_textBoxPosition = position
        ? position
        : std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Center);
}

void CommonComboBoxSkin::setButtonListSkin(
    const std::shared_ptr<ButtonListSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_buttonListSkin = skin;
    m_buttonListPosition = position
        ? position
        : std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Top,
            RelativeValue(RelType::Pixels, 0.0f),
            RelativeValue(RelType::Ratio, -1.0f));
}


std::shared_ptr<ToggleButton> CommonComboBoxSkin::createOpenButton() const
{
    if (!m_openButtonSkin)
        THROW_EX() << "Open button's skin isn't set";
    return std::make_shared<ToggleButton>(m_openButtonSkin, m_openButtonPosition);
}

std::shared_ptr<TextBox> CommonComboBoxSkin::createTextBox() const
{
    if (!m_textBoxSkin)
        THROW_EX() << "TextEdit's skin isn't set";
    auto result = std::make_shared<TextBox>(m_textBoxSkin, m_textBoxPosition);
    if (m_textBoxDisabled)
        result->setSelectionState(SelectionState::Disabled);
    return result;
}

std::shared_ptr<ButtonList> CommonComboBoxSkin::createList() const
{
    if (!m_buttonListSkin)
        THROW_EX() << "ButtonList's skin isn't set";
    return std::make_shared<ButtonList>(m_buttonListSkin, m_buttonListPosition);
}

void CommonComboBoxSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(&m_skinElements);
    builder->registerObject(m_openButtonSkin.get());
    builder->registerObject(m_textBoxSkin.get());
    builder->registerObject(m_buttonListSkin.get());
}

void CommonComboBoxSkin::serialize(Serializer& s) const
{
    s << "box" << m_box
        << "openButtonSkin" << m_openButtonSkin << "openButtonPosition" << m_openButtonPosition
        << "textBoxSkin" << m_textBoxSkin << "textBoxPosition" << m_textBoxPosition
        << "buttonListSkin" << m_buttonListSkin << "buttonListPosition" << m_buttonListPosition
        << "elements" << m_skinElements.objects() << "textBoxDisabled" << m_textBoxDisabled;
}

std::unique_ptr<IObject> deserializeCommonComboBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, openButtonSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, openButtonPosition);
    DESERIALIZE(std::shared_ptr<TextBoxSkin>, textBoxSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, textBoxPosition);
    DESERIALIZE(std::shared_ptr<ButtonListSkin>, buttonListSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, buttonListPosition);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);
    DESERIALIZE(bool, textBoxDisabled);

    std::unique_ptr<CommonComboBoxSkin> result(new CommonComboBoxSkin(box));
    result->setOpenButtonSkin(openButtonSkin, openButtonPosition);
    result->setTextBoxSkin(textBoxSkin, textBoxPosition);
    result->setButtonListSkin(buttonListSkin, buttonListPosition);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    result->setTextBoxDisabled(textBoxDisabled);
    return std::move(result);
}

REGISTER_CLASS(CommonComboBoxSkin);

} }
