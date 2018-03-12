/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/ComboBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ComboBox::ComboBox(
    const std::shared_ptr<ComboBoxSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_textBox(m_skin->createTextBox())
    , m_openButton(m_skin->createOpenButton())
    , m_nextID(0)
    , m_list(m_skin->createList())
{
    m_openButton->setName("openButton");
    m_openButton->setCallback([this]() { changeStateCallback(); });
    m_openButton->setParentPosition(this);
    
    m_textBox->setName("textBox");
    m_textBox->setParentPosition(this);
    
    m_list->setName("variants");
    m_list->setParentPosition(this);
    m_list->setAssociatedSelectable(m_openButton.get());
}

int ComboBox::currentVariantID() const
{
    const auto& curText = text();
    auto it = m_textToID.find(curText);
    if (it == m_textToID.end())
        return -1;
    return it->second;
}

std::vector<std::string> ComboBox::textVariants() const
{
    std::vector<std::string> result;
    result.reserve(m_idToText.size());
    for (auto it = m_idToText.begin(); it != m_idToText.end(); ++it)
        result.push_back(it->second);
    return std::move(result);
}

int ComboBox::addButton(const std::string& text, const std::shared_ptr<Button>& button)
{
    int id = m_nextID;
    addButton(text, button, m_nextID);
    return id;
}
    
void ComboBox::addButton(const std::string& text, const std::shared_ptr<Button>& button, unsigned int id)
{
    int textID = static_cast<int>(id);
    m_nextID = std::max(m_nextID, textID + 1);
    button->setCallback([this, textID]() { setTextFromVariant(textID); });
    m_list->addButton(button);

    {
        auto it = m_idToText.find(textID);
        if (it != m_idToText.end()) {
            m_textToID.erase(it->second);
            m_textToID[text] = textID;
            it->second = text;
            return;
        }
    }

    {
        auto it = m_textToID.find(text);
        if (it != m_textToID.end()) {
            m_idToText.erase(it->second);
            m_idToText[textID] = text;
            it->second = textID;
            return;
        }
    }

    m_idToText[textID] = text;
    m_textToID[text] = textID;
}

std::shared_ptr<IObject> ComboBox::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;

    auto transformedPoint = position().inversed() * point;
    if (m_openButton->isSelectableByPoint(transformedPoint))
        return m_openButton;
    if (m_textBox->isSelectableByPoint(transformedPoint))
        return m_textBox;
    return nullptr;
}

void ComboBox::loadResources()
{
    m_skin->loadResources();
    m_openButton->loadResources();
    m_textBox->loadResources();
    m_list->loadResources();
}

void ComboBox::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    m_textBox->draw(position);
    m_openButton->draw(position);
}

void ComboBox::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    BoundingBox fullBox = m_skin->box();
    setPositionBoxes(allowedBox, fullBox);

    m_openButton->setBox(fullBox);
    m_textBox->setBox(fullBox);
    m_list->setBox(fullBox);
}

BoundingBox ComboBox::box() const
{
    return m_skin->box();
}

void ComboBox::changeState(bool isOpened)
{
    if (isOpened) {
        m_buttonListSlot.init(this, this, m_list);
    } else {
        m_buttonListSlot.reset();
    }
}

void ComboBox::changeStateCallback()
{
    changeState(m_openButton->isPressed());
}

void ComboBox::setTextFromVariant(int id)
{
    auto it = m_idToText.find(id);
    if (it == m_idToText.end())
        return;
    m_textBox->setText(it->second);
    m_openButton->setSelectionState(SelectionState::None);
    if (m_callback)
        m_callbackHandle = Handle(m_callback);
}

void ComboBox::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject(m_skin.get());
    builder->registerObject(m_textBox.get());
    builder->registerObject(m_openButton.get());
    builder->registerObject(m_list.get());
}

void ComboBox::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "buttons" << m_list->buttons() << "textVariants" << textVariants();
}

std::unique_ptr<IObject> deserializeComboBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ComboBoxSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, buttons);
    DESERIALIZE(std::vector<std::string>, textVariants);
    std::unique_ptr<ComboBox> result(new ComboBox(skin, position));
    auto itText = textVariants.begin();
    for (auto it = buttons.begin(); it != buttons.end(); ++it, ++itText) {
        auto button = std::dynamic_pointer_cast<Button>(*it);
        if (!button)
            THROW_EX() << "ButtonList deserialization error: element is not button";
        result->addButton(*itText, button);
    }
    return std::move(result);
}

REGISTER_CLASS(ComboBox);

} }
