#include <stdafx.h>
#include <gamebase/engine/ComboBox.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/ObjectReflection.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

ComboBox::ComboBox(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ComboBoxSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_textBox(m_skin->createTextBox())
    , m_openButton(m_skin->createOpenButton())
    , m_nextID(0)
    , m_list(m_skin->createList())
    , m_isListOpened(false)
{
    m_openButton->setName("openButton");
    m_openButton->setCallback(std::bind(&ComboBox::changeState, this, true));
    m_openButton->setUnpressCallback(std::bind(&ComboBox::changeState, this, false));
    m_openButton->setParentPosition(this);
    
    m_textBox->setName("textEdit");
    m_textBox->setParentPosition(this);
    
    m_list->setName("variants");
    m_list->setParentPosition(this);
    m_list->setAssociatedSelectable(m_openButton.get());
}

int ComboBox::currentVariantID() const
{
    const auto& curText = text();
    auto it = std::find(m_textVariants.begin(), m_textVariants.end(), curText);
    if (it == m_textVariants.end())
        return -1;
    return *(m_textIDs.begin() + (it - m_textVariants.begin()));
}

void ComboBox::addButton(const std::string& text, const std::shared_ptr<Button>& button)
{
    addButton(text, button, m_nextID);
}
    
void ComboBox::addButton(const std::string& text, const std::shared_ptr<Button>& button, unsigned int id)
{
    int textID = static_cast<int>(id);
    m_nextID = std::max(m_nextID, textID + 1);
    button->setCallback(std::bind(&ComboBox::setTextFromVariant, this, textID));
    m_textVariants.push_back(text);
    m_textIDs.push_back(textID);
    m_list->addButton(button);
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
    if (m_isListOpened) {
        app->topViewLayout()->removeObject(m_buttonListID);
    } else {
        auto reflection = std::make_shared<ObjectReflection>(this, this);
        reflection->addObject(m_list);
        m_buttonListID = app->topViewLayout()->addObject(reflection);
    }
    m_isListOpened = isOpened;
}

void ComboBox::setTextFromVariant(int id)
{
    auto it = std::find(m_textIDs.begin(), m_textIDs.end(), id);
    if (it == m_textIDs.end())
        return;
    const auto& text = *(m_textVariants.begin() + (it - m_textIDs.begin()));
    m_textBox->setText(text);
    if (m_callback)
        m_callback(text, id);
    m_openButton->setSelectionState(SelectionState::None);
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
    s << "position" << m_offset << "skin" << m_skin << "buttons" << m_list->buttons() << "textVariants" << m_textVariants;
}

std::unique_ptr<IObject> deserializeComboBox(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ComboBoxSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, buttons);
    DESERIALIZE(std::vector<std::string>, textVariants);
    std::unique_ptr<ComboBox> result(new ComboBox(position, skin));
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

}
