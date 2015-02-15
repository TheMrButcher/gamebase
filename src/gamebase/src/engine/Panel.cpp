#include <stdafx.h>
#include <gamebase/engine/Panel.h>
#include <gamebase/engine/FloatPointingValue.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/graphics/Clipping.h>

namespace gamebase {

namespace {
class DragBarMovement : public FloatValue {
public:
    DragBarMovement(const std::shared_ptr<FloatValue>& controlledValue)
        : m_controlledValue(controlledValue)
    {}
        
    virtual float get() const override { THROW_EX() << "DragBarMovement::get() is unsupported"; }

    virtual void set(float value) override
    {
        if (value == 0.0f)
            m_start = m_controlledValue->get();
        else
            m_controlledValue->set(m_start + value);
    }

private:
    std::shared_ptr<FloatValue> m_controlledValue;
    float m_start;
};
}

class Panel::DragOffset : public IPositionable {
public:
    void reset() { m_offset = Vec2(); }

    std::shared_ptr<FloatValue> getX() { return std::make_shared<FloatPointingValue>(&m_offset.x); }
    std::shared_ptr<FloatValue> getY() { return std::make_shared<FloatPointingValue>(&m_offset.y); }

    virtual Transform2 position() const override { return ShiftTransform2(m_offset); }

private:
    Vec2 m_offset;
};

Panel::Panel(
    const std::shared_ptr<IRelativeOffset>& position,
    const std::shared_ptr<PanelSkin>& skin)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_dragOffset(std::make_shared<DragOffset>())
{
    m_objects.setParentPosition(this);
    m_selectionState = SelectionState::Disabled;

    if (auto dragBar = m_skin->createDragBar()) {
        auto horizontalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getX());
        auto verticalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getY());
        dragBar->setControlledHorizontal(horizontalDragCallback);
        dragBar->setControlledVertical(verticalDragCallback);
        addObject(dragBar);
    }

    if (auto closeButton = m_skin->createCloseButton()) {
        closeButton->setCallback(std::bind(&Panel::close, this));
        addObject(closeButton);
    }
}

void Panel::resetPosition()
{
    m_dragOffset->reset();
}

void Panel::close()
{
    setVisible(false);
    if (m_closeCallback)
        m_closeCallback();
}

Transform2 Panel::position() const
{
    return m_dragOffset->position() * OffsettedPosition::position();
}

IObject* Panel::find(
    const Vec2& point, const Transform2& globalPosition)
{
    if (!isVisible())
        return nullptr;

    auto fullPosition = position() * globalPosition;
    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_skin->box());
    bool isInBox = rectGeom.intersects(&pointGeom, position() * globalPosition, Transform2());
    if (m_skin->isLimitedByBox() && !isInBox)
        return nullptr;
    if (auto obj = m_objects.find(point, fullPosition))
        return obj;
    else
        return this;
        
}

void Panel::loadResources()
{
    m_skin->loadResources();
    m_objects.loadResources();
}

void Panel::drawAt(const Transform2& position) const
{
    m_skin->draw(position);

    if (m_skin->isLimitedByBox()) {
        setClipBox(position, m_skin->panelBox());
        m_objects.draw(position);
        disableClipping();
    } else {
        m_objects.draw(position);
    }
}

void Panel::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    m_offset->setBoxes(allowedBox, m_skin->box());
    m_objects.setBox(m_skin->panelBox());
}

BoundingBox Panel::box() const
{
    auto box = m_skin->box();
    if (!m_skin->isLimitedByBox())
        box.enlarge(m_objects.box());
    return box;
}

}
