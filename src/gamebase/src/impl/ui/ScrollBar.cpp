/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/ScrollBar.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/math/Math.h>

namespace gamebase { namespace impl {

class ScrollBar::DragBarMovement : public FloatValue {
public:
    DragBarMovement(ScrollBar* scrollBar)
        : m_scrollBar(scrollBar)
    {}
        
    virtual float get() const override { THROW_EX() << "DragBarMovement::get() is unsupported"; }

    virtual void set(const float& value) override
    {
        if (!m_scrollBar->m_controlledValue)
            return;
        if (value == 0.0f)
            m_start = m_scrollBar->m_controlledValue->get();
        else
            m_scrollBar->dragByPixels(m_start, value);
    }

private:
    ScrollBar* m_scrollBar;
    float m_start;
};

ScrollBar::ScrollBar(
    const std::shared_ptr<ScrollBarSkin>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(position)
    , Drawable(this)
    , m_skin(skin)
    , m_inited(false)
    , m_minVal(0)
    , m_maxVal(0)
    , m_visibleZoneSize(0)
    , m_dragBarOffset(std::make_shared<FixedOffset>())
    , m_dragBarCallback(std::make_shared<DragBarMovement>(this))
    , m_sizeFunc(m_skin->direction() == Direction::Horizontal ?
        &BoundingBox::width : &BoundingBox::height)
{
    m_collection.setParentPosition(this);
    if (auto decButton = skin->createDecButton()) {
        decButton->setCallback([this]() { decrease(); });
        m_collection.addObject(decButton);
    }
    if (auto incButton = skin->createIncButton()) {
        incButton->setCallback([this]() { increase(); });
        m_collection.addObject(incButton);
    }
    if (m_dragBar = skin->createDragBar(m_dragBarOffset)) {
        if (m_skin->direction() == Direction::Horizontal)
            m_dragBar->setControlledHorizontal(m_dragBarCallback);
        else
            m_dragBar->setControlledVertical(m_dragBarCallback);
        m_collection.addObject(m_dragBar);
    }
}

void ScrollBar::move(int numOfSteps)
{
    step(numOfSteps * m_skin->step());
}

void ScrollBar::loadResources()
{
    m_skin->loadResources();
    m_collection.loadResources();
    if (!m_inited) {
        m_inited = true;
        update();
        m_collection.loadResources();
    }
}

void ScrollBar::setBox(const BoundingBox& allowedBox)
{
    m_skin->setBox(allowedBox);
    auto box = m_skin->box();
    setPositionBoxes(allowedBox, box);
    m_collection.setBox(box);
    update();
}

void ScrollBar::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("skin", m_skin.get());
    builder->registerObject("objects", &m_collection);
}

void ScrollBar::serialize(Serializer& s) const
{
    s << "minValue" << m_minVal << "maxValue" << m_maxVal
        << "visibleZone" << m_visibleZoneSize
        << "position" << m_offset << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeScrollBar(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<ScrollBarSkin>, skin);
    DESERIALIZE(float, minValue);
    DESERIALIZE(float, maxValue);
    DESERIALIZE(float, visibleZone);
    std::unique_ptr<ScrollBar> result(new ScrollBar(skin, position));
    result->setRange(minValue, maxValue);
    result->setVisibleZoneSize(visibleZone);
    return std::move(result);
}

REGISTER_CLASS(ScrollBar);

void ScrollBar::decrease() { move(-1); }

void ScrollBar::increase() { move(+1); }

void ScrollBar::update()
{
    if (!m_inited)
        return;
    if (m_dragBar && box().isValid()) {
        auto dragBox = m_skin->dragBox();
        if (m_visibleZoneSize >= m_maxVal - m_minVal) {
            if (!m_skin->alwaysShow()) {
                setVisible(false);
            } else {
                m_dragBar->setSelectionState(SelectionState::Disabled);
            }
        } else {
            setVisible(true);
            float boxSize = (dragBox.*m_sizeFunc)();
            float dragBarSize = boxSize * m_visibleZoneSize / (m_maxVal - m_minVal);
            BoundingBox dragBarBox = m_skin->direction() == Direction::Horizontal
                ? BoundingBox(
                    Vec2(-0.5f * dragBarSize, -0.5f * dragBox.height()),
                    Vec2(0.5f * dragBarSize, 0.5f * dragBox.height()))
                : BoundingBox(
                    Vec2(-0.5f * dragBox.width(), -0.5f * dragBarSize),
                    Vec2(0.5f * dragBox.width(), 0.5f * dragBarSize));
            m_dragBar->setBox(dragBarBox);
            m_dragBar->setSelectionState(SelectionState::None);
        }
    }
    step(0.0f);
}

void ScrollBar::dragByPixels(float startValue, float offsetInPixels)
{
    float offset = offsetInPixels / (m_skin->dragBox().*m_sizeFunc)() * (m_maxVal - m_minVal);
    step(startValue + offset - m_controlledValue->get());
}

void ScrollBar::step(float value)
{
    if (!m_controlledValue)
        return;
    float curVal = m_controlledValue->get();
    m_controlledValue->set(clamp(
        curVal + value, m_minVal, std::max(m_minVal, m_maxVal - m_visibleZoneSize)));
    if (m_dragBar && m_dragBar->selectionState() != SelectionState::Disabled && box().isValid()) {
        auto dragBox = m_skin->dragBox();
        curVal = m_controlledValue->get();
        float midRatio = (curVal + 0.5f * m_visibleZoneSize) / (m_maxVal - m_minVal);
        Vec2 offset = m_skin->direction() == Direction::Horizontal
            ? Vec2(
                dragBox.bottomLeft.x + midRatio * dragBox.width(),
                0.5f * (dragBox.bottomLeft.y + dragBox.topRight.y))
            : Vec2(
                0.5f * (dragBox.bottomLeft.x + dragBox.topRight.x),
                dragBox.bottomLeft.y + midRatio * dragBox.height());
        m_dragBarOffset->update(offset);
    }
}

} }
