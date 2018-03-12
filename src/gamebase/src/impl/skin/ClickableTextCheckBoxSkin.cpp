/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/ClickableTextCheckBoxSkin.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

ClickableTextCheckBoxSkin::ClickableTextCheckBoxSkin(
    const std::shared_ptr<IRelativeBox>& labelBox,
    const std::shared_ptr<IRelativeGeometry>& geom)
    : AnimatedCheckBoxSkin(std::make_shared<OffsettedBox>(), geom)
    , m_labelBox(labelBox)
    , m_label(labelBox)
{
    AlignProperties alignProps = m_label.alignProperties();
    alignProps.horAlign = HorAlign::Left;
    alignProps.vertAlign = VertAlign::Center;
    alignProps.enableStacking = false;
    m_label.setAlignProperties(alignProps);
    m_label.setAdjustSize(true);
}

void ClickableTextCheckBoxSkin::setText(const std::string& text)
{
    m_label.setText(text);
    if (m_box->isValid()) {
        AnimatedCheckBoxSkin::setBox(m_label.box());
        AnimatedCheckBoxSkin::loadResources();
    }
}

void ClickableTextCheckBoxSkin::loadResources()
{
    AnimatedCheckBoxSkin::loadResources();
    m_label.loadResources();
}

void ClickableTextCheckBoxSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    AnimatedCheckBoxSkin::registerObject(builder);
    builder->registerObject("label", &m_label);
}
    
void ClickableTextCheckBoxSkin::serialize(Serializer& s) const
{
    AnimatedCheckBoxSkin::serialize(s);
    s << "labelBox" << m_labelBox
        << "text" << m_label.text()
        << "textColor" << m_label.color()
        << "font" << m_label.alignProperties().font;
}

std::unique_ptr<IObject> deserializeClickableTextCheckBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    DESERIALIZE(std::shared_ptr<IAnimation>, checkAnimation);
    DESERIALIZE(std::shared_ptr<IAnimation>, uncheckAnimation);
    DESERIALIZE(std::shared_ptr<IRelativeBox>, labelBox);
    DESERIALIZE(std::string, text);
    DESERIALIZE(GLColor, textColor);
    DESERIALIZE(FontDesc, font);

    std::unique_ptr<ClickableTextCheckBoxSkin> result(new ClickableTextCheckBoxSkin(labelBox, geometry));
    deserializeAnimatedObjectElements(deserializer, result.get());
    result->setCheckAnimation(checkAnimation);
    result->setUncheckAnimation(uncheckAnimation);
    result->setText(text);
    result->setTextColor(textColor);
    result->setFont(font);
    return std::move(result);
}

REGISTER_CLASS(ClickableTextCheckBoxSkin);

} }
