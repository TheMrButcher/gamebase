/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/SimpleToolTipSkin.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

SimpleToolTipSkin::SimpleToolTipSkin(
    const RelativeValue& maxWidth)
    : m_box(std::make_shared<RelativeBox>(maxWidth, RelativeValue()))
    , m_borderWidth(1)
    , m_padding(3)
    , m_label(std::make_shared<OffsettedBox>())
{
    m_border.setColor(GLColor(0, 0, 0, 1));
    m_fill.setColor(GLColor(1, 1, 1, 1));

    m_label.setColor(GLColor(0, 0, 0, 1));
    m_label.setAdjustSize(true);

    AlignProperties alignProps;
    alignProps.font = FontDesc();
    alignProps.enableStacking = true;
    alignProps.horAlign = HorAlign::Left;
    alignProps.vertAlign = VertAlign::Top;
    m_label.setAlignProperties(alignProps);
}

void SimpleToolTipSkin::setFont(const FontDesc& font)
{
    auto alignProps = m_label.alignProperties();
    alignProps.font = font;
    m_label.setAlignProperties(alignProps);
}

void SimpleToolTipSkin::setEnableStacking(bool enableStacking)
{
    auto alignProps = m_label.alignProperties();
    alignProps.enableStacking = enableStacking;
    m_label.setAlignProperties(alignProps);
}

void SimpleToolTipSkin::loadResources()
{
    m_border.loadResources();
    m_fill.loadResources();
    m_label.loadResources();
}

void SimpleToolTipSkin::drawAt(const Transform2& position) const
{
    m_border.draw(position);
    m_fill.draw(position);
    m_label.draw(position);
}

void SimpleToolTipSkin::setBox(const BoundingBox& allowedBox)
{
    m_box->setParentBox(allowedBox);

    m_label.setBox(m_box->get());
    auto box = m_label.box();

    auto innerBox = extend(box, m_padding);
    auto fullBox = extend(box, m_padding + m_borderWidth);
    if (!innerBox.isValid())
        innerBox = BoundingBox(box.center());
    if (!fullBox.isValid())
        fullBox = BoundingBox(box.center());
    m_border.setBox(fullBox);
    m_fill.setBox(innerBox);
}

void SimpleToolTipSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("tip", &m_label);
}

void SimpleToolTipSkin::serialize(Serializer& s) const
{
    s   << "padding" << m_padding << "fillColor" << m_fill.color()
        << "borderWidth" << m_borderWidth << "borderColor" << m_border.color()
        << "text" << m_label.text() << "textColor" << m_label.color()
        << "enableStacking" << m_label.alignProperties().enableStacking << "font" << m_label.alignProperties().font
        << "maxWidth" << m_box->widthValue();
}

std::unique_ptr<IObject> deserializeSimpleToolTipSkin(Deserializer& deserializer)
{
    DESERIALIZE(float, padding);
    DESERIALIZE(GLColor, fillColor);
    DESERIALIZE(float, borderWidth);
    DESERIALIZE(GLColor, borderColor);
    DESERIALIZE(std::string, text);
    DESERIALIZE(GLColor, textColor);
    DESERIALIZE(bool, enableStacking);
    DESERIALIZE(FontDesc, font);
    DESERIALIZE(RelativeValue, maxWidth);

    auto result = std::make_unique<SimpleToolTipSkin>(maxWidth);
    result->setPadding(padding);
    result->setFillColor(fillColor);
    result->setBorderWidth(borderWidth);
    result->setBorderColor(borderColor);
    result->setText(text);
    result->setTextColor(textColor);
    result->setEnableStacking(enableStacking);
    result->setFont(font);
    return std::move(result);
}

REGISTER_CLASS(SimpleToolTipSkin);

} }
