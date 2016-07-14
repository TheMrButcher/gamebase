/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/Panel.h>
#include <gamebase/impl/reg/ValueLink.h>
#include <gamebase/impl/geom/PointGeometry.h>
#include <gamebase/impl/geom/RectGeometry.h>
#include <gamebase/impl/graphics/Clipping.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <iterator>

namespace gamebase { namespace impl {

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
{
    m_objects.setParentPosition(this);
    m_sysObjects.setParentPosition(this);
    m_selectionState = SelectionState::Disabled;

    if (auto dragBar = m_skin->createDragBar()) {
        auto horizontalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getX());
        auto verticalDragCallback = std::make_shared<DragBarMovement>(m_dragOffset->getY());
        dragBar->setName("dragBar");
        dragBar->setControlledHorizontal(horizontalDragCallback);
        dragBar->setControlledVertical(verticalDragCallback);
        m_sysObjects.addObject(dragBar);
    }

    if (auto closeButton = m_skin->createCloseButton()) {
        closeButton->setCallback(std::bind(&Panel::close, this));
        closeButton->setName("closeButton");
        m_sysObjects.addObject(closeButton);
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

bool Panel::isSelectableByPoint(const Vec2& point) const
{
    if (!isVisible() || m_skin->isTransparent())
        return false;
    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_skin->box());
    return rectGeom.intersects(&pointGeom, position(), Transform2());
}

std::shared_ptr<IObject> Panel::findChildByPoint(const Vec2& point) const
{
    if (!isVisible())
        return nullptr;
    PointGeometry pointGeom(point);
    RectGeometry rectGeom(m_skin->box());
    bool isInBox = rectGeom.intersects(&pointGeom, position(), Transform2());
    if (m_skin->isLimitedByBox() && !isInBox)
        return nullptr;
    auto transformedPoint = position().inversed() * point;
    if (auto obj = m_sysObjects.findChildByPoint(transformedPoint))
        return obj;
    if (auto obj = m_objects.findChildByPoint(transformedPoint))
        return obj;
    return nullptr;
}

void Panel::loadResources()
{
    m_skin->loadResources();
    m_sysObjects.loadResources();
    m_objects.loadResources();
}

void Panel::drawAt(const Transform2& position) const
{
    m_skin->draw(position);
    if (m_skin->isLimitedByBox()) {
        pushClipBox(position, m_skin->panelBox());
        m_objects.draw(position);
        popClipBox();
    } else {
        m_objects.draw(position);
    }
    m_sysObjects.draw(position);
}

void Panel::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    setPositionBoxes(allowedBox, m_skin->box());
    m_sysObjects.setBox(m_skin->box());
    m_objects.setBox(m_skin->panelBox());
}

BoundingBox Panel::box() const
{
    auto box = m_skin->box();
    if (!m_skin->isLimitedByBox())
        box.add(m_objects.box());
    return box;
}

void Panel::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_skin.get());
    builder->registerObject("objects", &m_objects);
    builder->registerObject("sysObjects", &m_sysObjects);
}

void Panel::serialize(Serializer& s) const
{
    s << "position" << m_offset << "skin" << m_skin
        << "objects" << m_objects.objects();
}

std::unique_ptr<IObject> deserializePanel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<PanelSkin>, skin);
    DESERIALIZE(std::vector<std::shared_ptr<IObject>>, objects);
    std::unique_ptr<Panel> result(new Panel(position, skin));
    typedef std::map<int, std::shared_ptr<IObject>> IdToObj;
    for (auto it = objects.begin(); it != objects.end(); ++it)
        result->addObject(*it);
    return std::move(result);
}

REGISTER_CLASS(Panel);

} }
