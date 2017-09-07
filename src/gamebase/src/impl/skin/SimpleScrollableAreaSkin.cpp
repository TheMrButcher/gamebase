/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/SimpleScrollableAreaSkin.h>
#include <gamebase/impl/skin/impl/CommonScrollBarSkin.h>
#include <gamebase/impl/skin/tools/BaseSimpleButtonSkin.h>
#include <gamebase/impl/skin/impl/SimpleRectangleButtonSkin.h>
#include <gamebase/impl/drawobj/FilledTriangleDrawable.h>
#include <gamebase/impl/drawobj/StaticFilledRect.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/relbox/RelativeBox.h>
#include <gamebase/impl/relbox/SquareBox.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

namespace {
class ArrowButtonSkin : public BaseSimpleButtonSkin {
public:
    ArrowButtonSkin(const std::shared_ptr<IRelativeBox>& box)
        : BaseSimpleButtonSkin(box)
    {}

    enum ArrowDirection { Left, Right, Down, Up };
    void setArrowType(ArrowDirection direction)
    {
        switch (direction) {
        case Left: m_arrow.setPoints(Vec2(0.2f, -1), Vec2(-1, 0), Vec2(0.2f, 1)); break;
        case Right: m_arrow.setPoints(Vec2(-0.2f, -1), Vec2(1, 0), Vec2(-0.2f, 1)); break;
        case Down: m_arrow.setPoints(Vec2(-1, 0.2f), Vec2(0, -1), Vec2(1, 0.2f)); break;
        case Up: m_arrow.setPoints(Vec2(-1, -0.2f), Vec2(0, 1), Vec2(1, -0.2f)); break;
        }
    }

    void setArrowPadding(float padding) { m_arrowPadding = padding; }
    void setArrowColor(const Color& color) { m_arrow.setColor(color); }
    
    virtual void registerObject(PropertiesRegisterBuilder*) override {}

protected:
    virtual void loadContent() override
    {
        m_arrow.loadResources();
    }

    virtual void drawContent(const Transform2& position) const override
    {
        m_arrow.draw(position);
    }

    virtual void setContentBox(const BoundingBox& innerBox) override
    {
        m_arrow.setBox(extend(innerBox, -m_arrowPadding));
    }

    FilledTriangleDrawable m_arrow;
    float m_arrowPadding;
};

std::shared_ptr<IRelativeOffset> createScrollBarOffset(Direction::Enum direction)
{
    RelativeValue horOffset(RelType::Pixels, 0.0f);
    RelativeValue vertOffset(RelType::Ratio, -1.0f);
    if (direction == Direction::Vertical) {
        horOffset = RelativeValue();
        vertOffset = RelativeValue(RelType::Pixels, 0.0f);
    }
    return std::make_shared<AligningOffset>(
        direction == Direction::Horizontal ? HorAlign::Center : HorAlign::Left,
        direction == Direction::Horizontal ? VertAlign::Top : VertAlign::Center,
        horOffset, vertOffset);
}

std::shared_ptr<IRelativeBox> createScrollBarBox(Direction::Enum direction, float width)
{
    RelativeValue relWidth;
    RelativeValue relHeight(RelType::Pixels, width);
    if (direction == Direction::Vertical)
        std::swap(relWidth, relHeight);
    return std::shared_ptr<RelativeBox>(new RelativeBox(relWidth, relHeight));
}

std::shared_ptr<IRelativeBox> createDragBarBox(Direction::Enum direction, float width)
{
    RelativeValue relWidth(RelType::ValueMinusPixels, 2 * width);
    RelativeValue relHeight(RelType::Pixels, width);
    if (direction == Direction::Vertical)
        std::swap(relWidth, relHeight);
    return std::shared_ptr<RelativeBox>(new RelativeBox(relWidth, relHeight));
}
} // namespace

SimpleScrollableAreaSkin::SimpleScrollableAreaSkin(const std::shared_ptr<IRelativeBox>& box)
    : m_box(box)
    , m_scrollStep(10)
    , m_backgroundColor(0.9f, 0.9f, 0.9f)
    , m_borderWidth(1)
    , m_borderColor(0, 0, 0)
    , m_useGradient(true)
    , m_fillColor(0.7f, 0.7f, 0.7f)
    , m_arrowPadding(3.0f)
    , m_arrowColor(0.2f, 0.2f, 0.2f)
{
    m_scrollBarEnabled[0] = m_scrollBarEnabled[1] = true;
}

std::shared_ptr<ScrollBar> SimpleScrollableAreaSkin::createScrollBar(
    const std::shared_ptr<FloatValue>& controlledValue,
    Direction::Enum direction) const
{
    if (!m_scrollBarEnabled[direction])
        return nullptr;
    auto skin = std::make_shared<CommonScrollBarSkin>(
        createScrollBarBox(direction, m_scrollBarWidth),
        createDragBarBox(direction, m_scrollBarWidth),
        direction);
    skin->setAlwaysShow(false);
    skin->setStep(m_scrollStep);

    auto scrollBarBackground = std::make_shared<StaticFilledRect>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    scrollBarBackground->setColor(m_backgroundColor);
    skin->addElement(scrollBarBackground);
    
    auto decButtonSkin = std::make_shared<ArrowButtonSkin>(std::make_shared<SquareBox>());
    fillSkin(decButtonSkin.get());
    decButtonSkin->setArrowType(direction == Direction::Horizontal
        ? ArrowButtonSkin::Left : ArrowButtonSkin::Down);
    decButtonSkin->setArrowColor(m_arrowColor);
    decButtonSkin->setArrowPadding(m_arrowPadding);
    skin->setDecButtonSkin(decButtonSkin);

    auto incButtonSkin = std::make_shared<ArrowButtonSkin>(std::make_shared<SquareBox>());
    fillSkin(incButtonSkin.get());
    incButtonSkin->setArrowType(direction == Direction::Horizontal
        ? ArrowButtonSkin::Right : ArrowButtonSkin::Up);
    incButtonSkin->setArrowColor(m_arrowColor);
    incButtonSkin->setArrowPadding(m_arrowPadding);
    skin->setIncButtonSkin(incButtonSkin);

    auto dragBar = std::make_shared<SimpleRectangleButtonSkin>(
        std::make_shared<RelativeBox>(RelativeValue(), RelativeValue()));
    fillSkin(dragBar.get());
    skin->setDragBarSkin(dragBar);

    auto scrollBar = std::make_shared<ScrollBar>(skin, createScrollBarOffset(direction));
    scrollBar->setControlledValue(controlledValue);
    return scrollBar;
}

void SimpleScrollableAreaSkin::setSize(float width, float height)
{
    m_curAreaBox = m_box->get();
    BoundingBox areaWithScrollBars = m_box->get();
    if (m_scrollBarEnabled[Direction::Horizontal])
        areaWithScrollBars.bottomLeft.y += m_scrollBarWidth;
    if (m_scrollBarEnabled[Direction::Vertical])
        areaWithScrollBars.topRight.x -= m_scrollBarWidth;
    if (width > m_curAreaBox.width())
        m_curAreaBox.bottomLeft.y = areaWithScrollBars.bottomLeft.y;
    if (height > m_curAreaBox.height())
        m_curAreaBox.topRight.x = areaWithScrollBars.topRight.x;
    if (width > m_curAreaBox.width())
        m_curAreaBox.bottomLeft.y = areaWithScrollBars.bottomLeft.y;
}

void SimpleScrollableAreaSkin::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
}

void SimpleScrollableAreaSkin::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);
    m_curAreaBox = m_box->get();
}

void SimpleScrollableAreaSkin::serialize(Serializer& s) const
{
    s   << "horScrollBarEnabled" << m_scrollBarEnabled[Direction::Horizontal]
        << "vertScrollBarEnabled" << m_scrollBarEnabled[Direction::Vertical]
        << "scrollBarWidth" << m_scrollBarWidth
        << "scrollStep" << m_scrollStep
        << "backgroundColor" << m_backgroundColor
        << "borderWidth" << m_borderWidth
        << "borderColor" << m_borderColor
        << "useGradient" << m_useGradient
        << "fillColor" << m_fillColor
        << "arrowPadding" << m_arrowPadding
        << "arrowColor" << m_arrowColor
        << "box" << m_box;
}

std::unique_ptr<IObject> deserializeSimpleScrollableAreaSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(bool, horScrollBarEnabled);
    DESERIALIZE(bool, vertScrollBarEnabled);
    DESERIALIZE(float, scrollBarWidth);
    DESERIALIZE(float, scrollStep);
    DESERIALIZE(Color, backgroundColor);
    DESERIALIZE(float, borderWidth);
    DESERIALIZE(Color, borderColor);
    DESERIALIZE(bool, useGradient);
    DESERIALIZE(Color, fillColor);
    DESERIALIZE(float, arrowPadding);
    DESERIALIZE(Color, arrowColor);

    std::unique_ptr<SimpleScrollableAreaSkin> result(new SimpleScrollableAreaSkin(box));
    result->setScrollBarEnabled(Direction::Horizontal, horScrollBarEnabled);
    result->setScrollBarEnabled(Direction::Vertical, vertScrollBarEnabled);
    result->setScrollBarWidth(scrollBarWidth);
    result->setScrollStep(scrollStep);
    result->setBackgroundColor(backgroundColor);
    result->setBorderWidth(borderWidth);
    result->setBorderColor(borderColor);
    result->setUseGradient(useGradient);
    result->setFillColor(fillColor);
    result->setArrowPadding(arrowPadding);
    result->setArrowColor(arrowColor);
    return std::move(result);
}

REGISTER_CLASS(SimpleScrollableAreaSkin);

void SimpleScrollableAreaSkin::fillSkin(BaseSimpleButtonSkin* skin) const
{
    skin->setBorderWidth(m_borderWidth);
    skin->setBorderColor(m_borderColor);
    skin->setFillColor(m_fillColor);
    skin->setUseGradient(m_useGradient);
}

} }
