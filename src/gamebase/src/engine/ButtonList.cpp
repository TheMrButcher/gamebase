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
    virtual Transform2 transform() const override { return position(); }

private:
    Vec2 m_baseOffset;
    Vec2 m_offset;
};

ButtonList::ButtonList(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<ButtonListSkin>& skin)
    : OffsettedPosition(position)
    , m_skin(skin)
    , m_scrollOffset(std::make_shared<ScrollPosition>())
    , m_list(m_scrollOffset.get())
{
    m_list.setParentPosition(this);
    m_scroll = m_skin->createScrollBar(
        m_skin->direction() == Direction::Horizontal
            ? m_scrollOffset->getX()
            : m_scrollOffset->getY());
}

void ButtonList::addButton(const std::shared_ptr<Button>& button)
{
    button->setPosition(m_skin->createButtonOffset(m_list.size()));
    m_list.addChild(button);
}

IObject* ButtonList::find(
    const Vec2& point, const Transform2& globalPosition)
{
    auto fullPosition = position() * globalPosition;
    if (m_scroll) {
        if (auto result = m_scroll->find(point, fullPosition))
            return result;
    }

    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_box);
    if (!rectGeom.intersects(&pointGeom, transform() * globalPosition, Transform2()))
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

void ButtonList::draw(const Transform2& globalPosition) const
{
    auto position = transform() * globalPosition;
    m_skin->draw(position);

    setClipBox(position, m_box);
    m_list.draw(position);
    disableClipping();

    if (m_scroll)
        m_scroll->draw(position);
}

void ButtonList::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    BoundingBox fullBox = m_skin->box();
    m_offset->setBoxes(allowedBox, fullBox);

    m_box = m_skin->listBox();
    BoundingBox box = m_box;
    bool isHorizontal = m_skin->direction() == Direction::Horizontal;
    if (isHorizontal)
        box.topRight.x = std::numeric_limits<float>::min();
    else
        box.bottomLeft.y = std::numeric_limits<float>::min();
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        Button* button = dynamic_cast<Button*>(it->get());
        button->setBox(box);
        Vec2 buttonPos = button->position().offset;
        BoundingBox buttonBox = button->box();
        if (isHorizontal)
            box.bottomLeft.x = buttonBox.topRight.x + buttonPos.x;
        else
            box.topRight.y = buttonBox.bottomLeft.y + buttonPos.y;
    }

    if (m_scroll) {
        m_scroll->setBox(fullBox);
        auto fullSize = isHorizontal
            ? box.bottomLeft.x - m_box.bottomLeft.x
            : m_box.topRight.y - box.topRight.y;
        auto visibleSize = isHorizontal ? m_box.width() : m_box.height();
        Vec2 baseOffset = isHorizontal
            ? Vec2(0.0f, 0.0f)
            : Vec2(0.0f, fullSize - visibleSize);
        m_scrollOffset->init(baseOffset, baseOffset);
        m_scroll->setRange(0.0f, fullSize);
        m_scroll->setVisibleZoneSize(visibleSize);
    }
}

}
