#include <stdafx.h>
#include <gamebase/engine/TextList.h>
#include <gamebase/engine/Application.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/ObjectReflection.h>
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
    , m_nextID(0)
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

int TextList::currentVariantID() const
{
    const auto& curText = text();
    auto it = std::find(m_textVariants.begin(), m_textVariants.end(), curText);
    if (it == m_textVariants.end())
        return -1;
    return *(m_textIDs.begin() + (it - m_textVariants.begin()));
}

void TextList::addButton(const std::string& text, const std::shared_ptr<Button>& button)
{
    addButton(text, button, m_nextID);
}
    
void TextList::addButton(const std::string& text, const std::shared_ptr<Button>& button, unsigned int id)
{
    int textID = static_cast<int>(id);
    m_nextID = std::max(m_nextID, textID + 1);
    button->setCallback(std::bind(&TextList::setTextFromVariant, this, textID));
    m_textVariants.push_back(text);
    m_textIDs.push_back(textID);
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
}

void TextList::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    BoundingBox fullBox = m_skin->box();
    m_offset->setBoxes(allowedBox, fullBox);

    m_openButton->setBox(fullBox);
    m_textEdit->setBox(fullBox);
    m_list->setBox(fullBox);
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
    if (m_isListOpened) {
        app->topViewLayout()->removeObject(m_buttonListID);
    } else {
        auto reflection = std::make_shared<ObjectReflection>(this, this);
        reflection->addObject(m_list);
        m_buttonListID = app->topViewLayout()->addObject(reflection);
    }
    m_isListOpened = isOpened;
}

void TextList::setTextFromVariant(int id)
{
    auto it = std::find(m_textIDs.begin(), m_textIDs.end(), id);
    if (it == m_textIDs.end())
        return;
    const auto& text = *(m_textVariants.begin() + (it - m_textIDs.begin()));
    m_textEdit->setText(text);
    if (m_callback)
        m_callback(text, id);
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

std::unique_ptr<IObject> deserializeTextList(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<TextListSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, buttons);
    DESERIALIZE(std::vector<std::string>, textVariants);
    std::unique_ptr<TextList> result(new TextList(position, skin));
    auto itText = textVariants.begin();
    for (auto it = buttons.begin(); it != buttons.end(); ++it, ++itText) {
        auto button = std::dynamic_pointer_cast<Button>(*it);
        if (!button)
            THROW_EX() << "ButtonList deserialization error: element is not button";
        result->addButton(*itText, button);
    }
    return std::move(result);
}

REGISTER_CLASS(TextList);

}
