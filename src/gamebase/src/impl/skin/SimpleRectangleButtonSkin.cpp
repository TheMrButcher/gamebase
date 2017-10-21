/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/SimpleRectangleButtonSkin.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

SimpleRectangleButtonSkin::SimpleRectangleButtonSkin(
	const std::shared_ptr<IRelativeBox>& box)
    : BaseSimpleButtonSkin(box)
	, m_label(std::make_shared<OffsettedBox>())
{
	m_label.setColor(GLColor(0, 0, 0, 1));
	setFont(FontDesc());
}

void SimpleRectangleButtonSkin::setFont(const FontDesc& font)
{
	AlignProperties alignProps;
	alignProps.font = font;
	alignProps.enableStacking = false;
	alignProps.horAlign = HorAlign::Center;
	alignProps.vertAlign = VertAlign::Center;
	m_label.setAlignProperties(alignProps);
}

void SimpleRectangleButtonSkin::loadContent()
{
	m_label.loadResources();
}

void SimpleRectangleButtonSkin::drawContent(const Transform2& position) const
{
	m_label.draw(position);
}

void SimpleRectangleButtonSkin::setContentBox(const BoundingBox& innerBox)
{
	m_label.setBox(innerBox);
}

void SimpleRectangleButtonSkin::registerObject(PropertiesRegisterBuilder* builder)
{
	builder->registerObject("label", &m_label);
}

void SimpleRectangleButtonSkin::serialize(Serializer& s) const
{
    serializeParams(s);
	s   << "text" << m_label.text() << "textColor" << m_label.color() << "font" << m_label.alignProperties().font
		<< "box" << m_box;
}

std::unique_ptr<IObject> deserializeSimpleRectangleButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
	DESERIALIZE(std::string, text);
	DESERIALIZE(GLColor, textColor);
	DESERIALIZE(FontDesc, font);
    std::unique_ptr<SimpleRectangleButtonSkin> result(new SimpleRectangleButtonSkin(box));
    deserializeBaseSimpleButtonSkin(result.get(), deserializer);
	result->setText(text);
	result->setTextColor(textColor);
	result->setFont(font);
    return std::move(result);
}

REGISTER_CLASS(SimpleRectangleButtonSkin);

} }
