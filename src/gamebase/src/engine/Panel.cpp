#include <stdafx.h>
#include <gamebase/engine/Panel.h>
#include <gamebase/engine/ValueLink.h>
#include <gamebase/geom/PointGeometry.h>
#include <gamebase/geom/RectGeometry.h>
#include <gamebase/graphics/Clipping.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>
#include <iterator>

namespace gamebase {

namespace {
class DragBarMovement : public FloatValue {
public:
    DragBarMovement(const std::shared_ptr<FloatValue>& controlledValue)
        : m_controlledValue(controlledValue)
    {}
        
    virtual float get() const override { THROW_EX() << "DragBarMovement::get() is unsupported"; }

    virtual void set(const float& value) override
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

    std::shared_ptr<FloatValue> getX() { return std::make_shared<ValueLink<float>>(&m_offset.x); }
    std::shared_ptr<FloatValue> getY() { return std::make_shared<ValueLink<float>>(&m_offset.y); }

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
    , m_sysObjectsNum(0)
    , m_transparent(false)
{
    m_objects.setParentPosition(this);
    m_selectionState = SelectionState::Disabled;

    if (auto dragBar = m_skin->createDragBar()) {
        auto horizontalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getX());
        auto verticalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getY());
        dragBar->setName("dragBar");
        dragBar->setControlledHorizontal(horizontalDragCallback);
        dragBar->setControlledVertical(verticalDragCallback);
        addObject(dragBar);
        ++m_sysObjectsNum;
    }

    if (auto closeButton = m_skin->createCloseButton()) {
        closeButton->setCallback(std::bind(&Panel::close, this));
        closeButton->setName("closeButton");
        addObject(closeButton);
        ++m_sysObjectsNum;
    }
}

void Panel::resetPosition()
{
    m_dragOffset->reset();
}

void Panel::close()
{
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
    if (m_transparent)
        return nullptr;
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

void Panel::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_skin.get());
    builder->registerObject("objects", &m_objects);
}

void Panel::serialize(Serializer& s) const
{
    std::vector<std::shared_ptr<IObject>> objects;
    std::copy(m_objects.begin() + m_sysObjectsNum, m_objects.end(),
        std::back_inserter(objects));
    
    s << "position" << m_offset << "skin" << m_skin << "objects" << objects << "transparent" << m_transparent;
}

std::unique_ptr<IObject> deserializePanel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<PanelSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, objects);
    DESERIALIZE(bool, transparent);
    std::unique_ptr<Panel> result(new Panel(position, skin));
    typedef std::map<int, std::shared_ptr<IObject>> IdToObj;
    for (auto it = objects.begin(); it != objects.end(); ++it)
        result->addObject(*it);
    result->setTransparent(transparent);
    return std::move(result);
}

REGISTER_CLASS(Panel);

}
