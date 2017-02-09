/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/RectangleImageButtonSkin.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

RectangleImageButtonSkin::RectangleImageButtonSkin(
	const std::shared_ptr<IRelativeBox>& box)
    : BaseSimpleButtonSkin(box)
	, m_icon(std::make_shared<PixelBox>(32.0f, 32.0f))
{}

void RectangleImageButtonSkin::loadContent()
{
	m_icon.loadResources();
}

void RectangleImageButtonSkin::drawContent(const Transform2& position) const
{
	m_icon.draw(position);
}

void RectangleImageButtonSkin::setContentBox(const BoundingBox& innerBox)
{
	m_icon.setBox(innerBox);
}

void RectangleImageButtonSkin::serialize(Serializer& s) const
{
    serializeParams(s);
    BoundingBox iconBox = m_icon.relativeBox()->count(BoundingBox());
	s   << "imageName" << m_icon.imageName() << "imageWidth" << iconBox.width()
        << "imageHeight" << iconBox.height() << "imageColor" << m_icon.color()
		<< "box" << m_box;
}

std::unique_ptr<IObject> deserializeRectangleImageButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
	DESERIALIZE(std::string, imageName);
    DESERIALIZE(float, imageWidth);
    DESERIALIZE(float, imageHeight);
	DESERIALIZE(Color, imageColor);
    std::unique_ptr<RectangleImageButtonSkin> result(new RectangleImageButtonSkin(box));
    deserializeBaseSimpleButtonSkin(result.get(), deserializer);
    result->setImageName(imageName);
    result->setImageSizes(imageWidth, imageHeight);
    result->setImageColor(imageColor);
    return std::move(result);
}

REGISTER_CLASS(RectangleImageButtonSkin);

} }
