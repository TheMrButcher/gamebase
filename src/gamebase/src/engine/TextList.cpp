#include <stdafx.h>
#include <gamebase/engine/TextList.h>

namespace gamebase {

TextList::TextList(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<TextListSkin>& skin)
    : OffsettedPosition(position)
    , m_skin(skin)
    , m_textEdit(m_skin->createTextEdit())
    , m_openButton(m_skin->createOpenButton())
    , m_list(m_skin->createList())
    , m_isListOpened(false)
{
    m_openButton->setCallback(std::bind(&TextList::changeState, this, true));
    m_openButton->setUnpressCallback(std::bind(&TextList::changeState, this, false));

    m_textEdit->setParentPosition(this);
    m_openButton->setParentPosition(this);
    m_list->setParentPosition(this);
    m_list->setAssociatedSelectable(m_openButton.get());
}

void TextList::addButton(const std::string& text, const std::shared_ptr<Button>& button)
{
    button->setCallback(std::bind(&TextList::setTextFromVariant, this, text));
    m_list->addButton(button);
}

IObject* TextList::find(
    const Vec2& point, const Transform2& globalPosition)
{
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

void TextList::draw(const Transform2& globalPosition) const
{
    auto fullTransform = transform() * globalPosition;
    m_skin->draw(fullTransform);
    m_textEdit->draw(fullTransform);
    m_openButton->draw(fullTransform);
    if (m_isListOpened)
        m_list->draw(fullTransform);
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

}
