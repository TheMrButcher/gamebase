/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include "TextureHelpers.h"
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/graphics/Image.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

void StaticTextureRect::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;

    if (!m_parentBox.isValid())
        return;
    setBox(m_parentBox);
    loadResources();
}

GLTexture StaticTextureRect::loadTextureImpl(const std::string& imageName)
{
    if (imageName.empty())
        return loadTexture(DEFAULT_IMAGE_ID, &defaultImage);
	return loadTexture(imageName, [imageName]() { return loadImageFromFile(imageName); });
}

void StaticTextureRect::serialize(Serializer& s) const
{
    s << "color" << m_color << "imageName" << m_imageName << "box" << m_box << "position" << m_offset;
}

std::unique_ptr<IObject> deserializeStaticTextureRect(Deserializer& deserializer)
{
    return deserializeTextureBase<StaticTextureRect>(deserializer);
}

REGISTER_CLASS(StaticTextureRect);

void StaticTextureRect::loadTextureImpl()
{
    m_texture = loadTextureImpl(m_imageName);
}

} }
