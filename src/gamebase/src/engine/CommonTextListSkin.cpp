#include <stdafx.h>
#include <gamebase/engine/CommonTextListSkin.h>
#include <gamebase/engine/AligningOffset.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

CommonTextListSkin::CommonTextListSkin(
    const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
{}

void CommonTextListSkin::setOpenButtonSkin(
    const std::shared_ptr<ButtonSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_openButtonSkin = skin;
    m_openButtonPosition = position
        ? position
        : std::make_shared<AligningOffset>(
            HorAlign::Right, VertAlign::Center);
}

void CommonTextListSkin::setTextEditSkin(
    const std::shared_ptr<TextEditSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
{
    m_textEditSkin = skin;
    m_textEditPosition = position
        ? position
        : std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Center);
}

void CommonTextListSkin::setButtonListSkin(
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


std::shared_ptr<PressableButton> CommonTextListSkin::createOpenButton() const
{
    if (!m_openButtonSkin)
        THROW_EX() << "Open button's skin isn't set";
    return std::make_shared<PressableButton>(m_openButtonPosition, m_openButtonSkin);
}

std::shared_ptr<TextEdit> CommonTextListSkin::createTextEdit() const
{
    if (!m_textEditSkin)
        THROW_EX() << "TextEdit's skin isn't set";
    return std::make_shared<TextEdit>(m_textEditPosition, m_textEditSkin);
}

std::shared_ptr<ButtonList> CommonTextListSkin::createList() const
{
    if (!m_buttonListSkin)
        THROW_EX() << "ButtonList's skin isn't set";
    return std::make_shared<ButtonList>(m_buttonListPosition, m_buttonListSkin);
}

void CommonTextListSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("elements", &m_skinElements);
    builder->registerObject("openButton", m_openButtonSkin.get());
    builder->registerObject("textEdit", m_textEditSkin.get());
    builder->registerObject("buttonList", m_buttonListSkin.get());
}

void CommonTextListSkin::serialize(Serializer& s) const
{
    s << "box" << m_box
        << "openButtonSkin" << m_openButtonSkin << "openButtonPosition" << m_openButtonPosition
        << "textEditSkin" << m_textEditSkin << "textEditPosition" << m_textEditPosition
        << "buttonListSkin" << m_buttonListSkin << "buttonListPosition" << m_buttonListPosition
        << "elements" << m_skinElements.objects();
}

std::unique_ptr<IObject> deserializeCommonTextListSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<ButtonSkin>, openButtonSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, openButtonPosition);
    DESERIALIZE(std::shared_ptr<TextEditSkin>, textEditSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, textEditPosition);
    DESERIALIZE(std::shared_ptr<ButtonListSkin>, buttonListSkin);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, buttonListPosition);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, elements);

    std::unique_ptr<CommonTextListSkin> result(new CommonTextListSkin(box));
    result->setOpenButtonSkin(openButtonSkin, openButtonPosition);
    result->setTextEditSkin(textEditSkin, textEditPosition);
    result->setButtonListSkin(buttonListSkin, buttonListPosition);
    for (auto it = elements.begin(); it != elements.end(); ++it)
        result->addElement(*it);
    return std::move(result);
}

REGISTER_CLASS(CommonTextListSkin);

}
