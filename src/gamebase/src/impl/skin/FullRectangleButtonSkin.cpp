/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/FullRectangleButtonSkin.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relpos/AligningOffset.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

FullRectangleButtonSkin::FullRectangleButtonSkin(
	const std::shared_ptr<IRelativeBox>& box)
    : SimpleRectangleButtonSkin(box)
	, m_icon(std::make_shared<PixelBox>(32.0f, 32.0f))
{
    m_label.setAdjustSize(true);
    setContentPositioning(8.0f, Direction::Horizontal);
}

void FullRectangleButtonSkin::setContentPositioning(float padding, Direction::Enum direction)
{
    m_padding = padding;
    m_direction = direction;
    if (m_direction == Direction::Horizontal) {
        m_icon.setRelativeOffset(std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Center,
            RelativeValue(RelType::Pixels, m_padding),
            RelativeValue(RelType::Pixels, 0)));
        m_label.setRelativeOffset(std::make_shared<AligningOffset>(
            HorAlign::Left, VertAlign::Center,
            RelativeValue(RelType::Pixels, m_padding),
            RelativeValue(RelType::Pixels, 0)));
    } else {
        m_icon.setRelativeOffset(std::make_shared<AligningOffset>(
            HorAlign::Center, VertAlign::Top,
            RelativeValue(RelType::Pixels, 0),
            RelativeValue(RelType::Pixels, -m_padding)));
        m_label.setRelativeOffset(std::make_shared<AligningOffset>(
            HorAlign::Center, VertAlign::Bottom,
            RelativeValue(RelType::Pixels, 0),
            RelativeValue(RelType::Pixels, m_padding)));
    }
}

void FullRectangleButtonSkin::loadContent()
{
    SimpleRectangleButtonSkin::loadContent();
	m_icon.loadResources();
}

void FullRectangleButtonSkin::drawContent(const Transform2& position) const
{
    SimpleRectangleButtonSkin::drawContent(position);
	m_icon.draw(position);
}

void FullRectangleButtonSkin::setContentBox(const BoundingBox& innerBox)
{
	m_icon.setBox(innerBox);
    if (m_direction == Direction::Horizontal) {
        auto box = innerBox;
        box.bottomLeft.x = m_icon.movedBox().right();
        m_label.setBox(box);
    } else {
        m_label.setBox(innerBox);
    }
}

void FullRectangleButtonSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    SimpleRectangleButtonSkin::registerObject(builder);
    builder->registerObject("icon", &m_icon);
}

void FullRectangleButtonSkin::serialize(Serializer& s) const
{
    serializeParams(s);
    BoundingBox iconBox = m_icon.relativeBox()->count(BoundingBox());
    s   << "padding" << m_padding << "direction" << m_direction
        << "imageName" << m_icon.imageName() << "imageWidth" << iconBox.width()
        << "imageHeight" << iconBox.height() << "imageColor" << m_icon.color()
        << "text" << m_label.text() << "textColor" << m_label.color() << "font" << m_label.alignProperties().font
		<< "box" << m_box;
}

std::unique_ptr<IObject> deserializeFullRectangleButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(float, padding);
    DESERIALIZE(Direction::Enum, direction);
	DESERIALIZE(std::string, imageName);
    DESERIALIZE(float, imageWidth);
    DESERIALIZE(float, imageHeight);
	DESERIALIZE(GLColor, imageColor);
	DESERIALIZE(std::string, text);
	DESERIALIZE(GLColor, textColor);
	DESERIALIZE(FontDesc, font);
    std::unique_ptr<FullRectangleButtonSkin> result(new FullRectangleButtonSkin(box));
    deserializeBaseSimpleButtonSkin(result.get(), deserializer);
    result->setContentPositioning(padding, direction);
    result->setImageName(imageName);
    result->setImageSizes(imageWidth, imageHeight);
    result->setImageColor(imageColor);
	result->setText(text);
	result->setTextColor(textColor);
	result->setFont(font);
    return std::move(result);
}

REGISTER_CLASS(FullRectangleButtonSkin);

} }
