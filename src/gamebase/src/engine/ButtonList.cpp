#include <stdafx.h>
#include <gamebase/engine/ButtonList.h>
#include <gamebase/engine/FloatPointingValue.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/graphics/Clipping.h>

namespace gamebase {

class ButtonList::ScrollPosition : public IPositionable {
public:
    void init(const Vec2& base, const Vec2& initialOffset)
    {
        m_baseOffset = base;
        m_offset = initialOffset;
    }

    std::shared_ptr<FloatValue> getX() { return std::make_shared<FloatPointingValue>(&m_offset.x); }
    std::shared_ptr<FloatValue> getY() { return std::make_shared<FloatPointingValue>(&m_offset.y); }

    virtual Transform2 position() const override { return ShiftTransform2(m_baseOffset - m_offset); }

private:
    Vec2 m_baseOffset;
    Vec2 m_offset;
};

ButtonList::ButtonList(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ButtonListSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_scrollOffset(std::make_shared<ScrollPosition>())
    , m_list(m_scrollOffset.get())
{
    m_list.setParentPosition(this);
    if (m_scroll = m_skin->createScrollBar(
        m_skin->direction() == Direction::Horizontal
            ? m_scrollOffset->getX()
            : m_scrollOffset->getY())) {
        m_scroll->setParentPosition(this);
    }
}

void ButtonList::addButton(const std::shared_ptr<Button>& button)
{
    button->setPosition(m_skin->createButtonOffset(m_list.size()));
    m_list.addObject(button);
}

void ButtonList::setAssociatedSelectable(ISelectable* selectable)
{
    m_list.setAssociatedSelectable(selectable);
    if (m_scroll)
        m_scroll->setAssociatedSelectable(selectable);
}

IObject* ButtonList::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!isVisible())
        return nullptr;

    auto fullPosition = position() * globalPosition;
    if (m_scroll) {
        if (auto result = m_scroll->find(point, fullPosition))
            return result;
    }

    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_box);
    if (!rectGeom.intersects(&pointGeom, position() * globalPosition, Transform2()))
        return nullptr;
    return m_list.find(point, fullPosition);
}

void ButtonList::loadResources()
{
    m_skin->loadResources();
    m_list.loadResources();
    if (m_scroll)
        m_scroll->loadResources();
}

void ButtonList::drawAt(const Transform2& position) const
{
    m_skin->draw(position);

    setClipBox(position, m_box);
    m_list.draw(position);
    disableClipping();

    if (m_scroll)
        m_scroll->draw(position);
}

namespace {
float placeObjects(ObjectsCollection& collection, const BoundingBox& originBox, bool isHorizontal)
{
    BoundingBox box = originBox;
    if (isHorizontal)
        box.topRight.x = std::numeric_limits<float>::min();
    else
        box.bottomLeft.y = std::numeric_limits<float>::min();
    for (auto it = collection.begin(); it != collection.end(); ++it) {
        Button* button = dynamic_cast<Button*>(it->get());
        button->setBox(box);
        Vec2 buttonPos = button->position().offset;
        BoundingBox buttonBox = button->box();
        if (isHorizontal)
            box.bottomLeft.x = buttonBox.topRight.x + buttonPos.x;
        else
            box.topRight.y = buttonBox.bottomLeft.y + buttonPos.y;
    }
    return isHorizontal
        ? box.bottomLeft.x - originBox.bottomLeft.x
        : originBox.topRight.y - box.topRight.y;
}
}

void ButtonList::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);

    bool isHorizontal = m_skin->direction() == Direction::Horizontal;
    float fullSize = placeObjects(m_list, m_skin->listBox(), isHorizontal);
    m_skin->setMaxSize(fullSize);
    m_box = m_skin->listBox();
    placeObjects(m_list, m_skin->listBox(), isHorizontal);
    m_offset->setBoxes(allowedBox, m_skin->box());

    if (m_scroll) {
        m_scroll->setBox(m_skin->box());
        auto visibleSize = isHorizontal ? m_box.width() : m_box.height();
        Vec2 baseOffset = isHorizontal
            ? Vec2(0.0f, 0.0f)
            : Vec2(0.0f, std::max(0.0f, fullSize - visibleSize));
        m_scrollOffset->init(baseOffset, baseOffset);
        m_scroll->setRange(0.0f, fullSize);
        m_scroll->setVisibleZoneSize(visibleSize);
    }
}

}
