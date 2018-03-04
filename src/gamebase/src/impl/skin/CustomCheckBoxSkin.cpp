/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#include <stdafx.h>
#include <gamebase/impl/skin/impl/CustomCheckBoxSkin.h>
#include <gamebase/impl/relbox/PixelBox.h>
#include <gamebase/impl/relbox/OffsettedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

CustomCheckBoxSkin::CustomCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box)
    : BaseCheckBoxSkin(box)
    , m_background(std::make_shared<OffsettedBox>())
    , m_check(std::make_shared<PixelBox>(32.0f, 32.0f))
{}

void CustomCheckBoxSkin::loadResources()
{
    m_background.loadResources();
    m_check.loadResources();
}

void CustomCheckBoxSkin::registerObject(PropertiesRegisterBuilder* builder)
{
    builder->registerObject("checkBG", &m_background);
    builder->registerObject("check", &m_check);
}

void CustomCheckBoxSkin::serialize(Serializer& s) const
{
    BoundingBox checkBox = m_check.relativeBox()->count(BoundingBox());
    s   << "backgroundImageName" << m_background.imageName() << "backgroundImageColor" << m_background.color()
        << "checkImageName" << m_check.imageName() << "checkImageWidth" << checkBox.width()
        << "checkImageHeight" << checkBox.height() << "checkImageColor" << m_check.color()
        << "box" << m_box;
}

void CustomCheckBoxSkin::drawBackground(const Transform2& position) const
{
    m_background.draw(position);
}

void CustomCheckBoxSkin::drawCheck(const Transform2& position) const
{
    m_check.draw(position);
}

void CustomCheckBoxSkin::setContentBox(const BoundingBox& box)
{
    m_background.setBox(box);
    m_check.setBox(box);
}

std::unique_ptr<IObject> deserializeCustomCheckBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::string, backgroundImageName);
    DESERIALIZE(GLColor, backgroundImageColor);
    DESERIALIZE(std::string, checkImageName);
    DESERIALIZE(float, checkImageWidth);
    DESERIALIZE(float, checkImageHeight);
    DESERIALIZE(GLColor, checkImageColor);
    auto result = std::make_unique<CustomCheckBoxSkin>(box);
    result->setBackgroundImageName(backgroundImageName);
    result->setBackgroundImageColor(backgroundImageColor);
    result->setCheckImageName(checkImageName);
    result->setCheckImageSizes(checkImageWidth, checkImageHeight);
    result->setCheckImageColor(checkImageColor);
    return std::move(result);
}

REGISTER_CLASS(CustomCheckBoxSkin);

} }
