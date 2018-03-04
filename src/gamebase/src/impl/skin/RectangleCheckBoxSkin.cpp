/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/RectangleCheckBoxSkin.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

RectangleCheckBoxSkin::RectangleCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box)
    : BaseCheckBoxSkin(box)
    , m_borderWidth(1.f)
    , m_check(std::make_shared<PixelBox>(32.0f, 32.0f))
{
    m_border.setColor(GLColor(0, 0, 0, 1));
    m_fill.setColor(GLColor(0.7f, 0.7f, 0.7f, 1));
}

void RectangleCheckBoxSkin::loadResources()
{
    m_border.loadResources();
    m_fill.loadResources();
    m_check.loadResources();
}

void RectangleCheckBoxSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("check", &m_check);
}

void RectangleCheckBoxSkin::serialize(Serializer& s) const
{
    BoundingBox checkBox = m_check.relativeBox()->count(BoundingBox());
    s   << "fillColor" << m_fill.color()
        << "borderWidth" << m_borderWidth << "borderColor" << m_border.color()
        << "checkImageName" << m_check.imageName() << "checkImageWidth" << checkBox.width()
        << "checkImageHeight" << checkBox.height() << "checkImageColor" << m_check.color()
        << "box" << m_box;
}

void RectangleCheckBoxSkin::drawBackground(const Transform2& position) const
{
    m_border.draw(position);
    m_fill.draw(position);
}

void RectangleCheckBoxSkin::drawCheck(const Transform2& position) const
{
    m_check.draw(position);
}

void RectangleCheckBoxSkin::setContentBox(const BoundingBox& box)
{
    BoundingBox innerBox = extend(box, -m_borderWidth);
    if (!innerBox.isValid())
        innerBox = BoundingBox(box.center());
    m_border.setBox(box);
    m_fill.setBox(innerBox);
    m_check.setBox(innerBox);
}

std::unique_ptr<IObject> deserializeRectangleCheckBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(GLColor, fillColor);
    DESERIALIZE(float, borderWidth);
    DESERIALIZE(GLColor, borderColor);
    DESERIALIZE(std::string, checkImageName);
    DESERIALIZE(float, checkImageWidth);
    DESERIALIZE(float, checkImageHeight);
    DESERIALIZE(GLColor, checkImageColor);
    auto result = std::make_unique<RectangleCheckBoxSkin>(box);
    result->setFillColor(fillColor);
    result->setBorderWidth(borderWidth);
    result->setBorderColor(borderColor);
    result->setCheckImageName(checkImageName);
    result->setCheckImageSizes(checkImageWidth, checkImageHeight);
    result->setCheckImageColor(checkImageColor);
    return std::move(result);
}

REGISTER_CLASS(RectangleCheckBoxSkin);

} }
