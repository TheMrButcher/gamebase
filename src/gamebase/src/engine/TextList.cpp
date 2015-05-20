#include <stdafx.h>
#include <gamebase/engine/TextList.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

TextList::TextList(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<TextListSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_textEdit(m_skin->createTextEdit())
    , m_openButton(m_skin->createOpenButton())
    , m_list(m_skin->createList())
    , m_isListOpened(false)
{
    m_openButton->setName("openButton");
    m_openButton->setCallback(std::bind(&TextList::changeState, this, true));
    m_openButton->setUnpressCallback(std::bind(&TextList::changeState, this, false));
    m_openButton->setParentPosition(this);
    
    m_textEdit->setName("textEdit");
    m_textEdit->setParentPosition(this);
    
    m_list->setName("variants");
    m_list->setParentPosition(this);
    m_list->setAssociatedSelectable(m_openButton.get());
}

void TextList::addButton(const std::string& text, const std::shared_ptr<Button>& button)
{
    button->setCallback(std::bind(&TextList::setTextFromVariant, this, text));
    m_textVariants.push_back(text);
    m_list->addButton(button);
}

IObject* TextList::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!isVisible())
        return nullptr;

    auto fullPosition = position() * globalPosition;
    if (auto findable = m_openButton->find(point, fullPosition))
        return findable;
    if (auto findable = m_textEdit->find(point, fullPosition))
        return findable;
    if (m_isListOpened) {
        if (auto findable = m_list->find(point, fullPosition))
            return findable;
    }
    return nullptr;
}

void TextList::loadResources()
{
    m_skin->loadResources();
    m_openButton->loadResources();
    m_textEdit->loadResources();
    m_list->loadResources();
}

void TextList::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    m_textEdit->draw(position);
    m_openButton->draw(position);
    if (m_isListOpened)
        m_list->draw(position);
}

void TextList::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    BoundingBox fullBox = m_skin->box();
    m_offset->setBoxes(allowedBox, fullBox);

    m_openButton->setBox(fullBox);
    m_textEdit->setBox(fullBox);
    m_list->setBox(m_skin->listBox());
}

BoundingBox TextList::box() const
{
    BoundingBox box = m_skin->box();
    if (m_isListOpened)
        box.enlarge(m_list->box().move(m_list->position().offset));
    return box;
}

void TextList::changeState(bool isOpened)
{
    m_isListOpened = isOpened;
}

void TextList::setTextFromVariant(const std::string& text)
{
    m_textEdit->setText(text);
    m_openButton->setSelectionState(SelectionState::None);
}

void TextList::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_skin.get());
    builder->registerObject(m_textEdit.get());
    builder->registerObject(m_openButton.get());
    builder->registerObject(m_list.get());
}

void TextList::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin << "buttons" << m_list->buttons() << "textVariants" << m_textVariants;
}

IObject* deserializeTextList(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<TextListSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, buttons);
    DESERIALIZE(std::vector<std::string>, textVariants);
    auto* result = new TextList(position, skin);
    auto itText = textVariants.begin();
    for (auto it = buttons.begin(); it != buttons.end(); ++it, ++itText) {
        auto button = std::dynamic_pointer_cast<Button>(*it);
        if (!button) {
            delete result;
            THROW_EX() << "ButtonList deserialization error: element is not button";
        }
        result->addButton(*itText, button);
    }
    return result;
}

REGISTER_CLASS(TextList);

}
