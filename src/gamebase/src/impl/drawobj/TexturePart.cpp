/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TexturePart.h>
#include "TextureHelpers.h"
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

TexturePart::TexturePart(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : StaticTextureRect(box, position)
    , m_texMin(0, 0)
    , m_texMax(1, 1)
{}

void TexturePart::registerObject(PropertiesRegisterBuilder* builder)
{
    StaticTextureRect::registerObject(builder);
    std::function<void()> updater = std::bind(&TexturePart::update, this);
    builder->registerProperty("min", &m_texMin, updater);
    builder->registerProperty("x0", &m_texMin.x, updater);
    builder->registerProperty("y0", &m_texMin.y, updater);
    builder->registerProperty("max", &m_texMax, updater);
    builder->registerProperty("x1", &m_texMax.x, updater);
    builder->registerProperty("y1", &m_texMax.y, updater);
}

void TexturePart::serialize(Serializer& s) const
{
    StaticTextureRect::serialize(s);
    s << "texMin" << m_texMin << "texMax" << m_texMax;
}

std::unique_ptr<IObject> deserializeTexturePart(Deserializer& deserializer)
{
    DESERIALIZE(Vec2, texMin);
    DESERIALIZE(Vec2, texMax);
    auto result = deserializeTextureBase<TexturePart>(deserializer);
    result->setTexMin(texMin);
    result->setTexMax(texMax);
    return std::move(result);
}

REGISTER_CLASS(TexturePart);

void TexturePart::updateImpl()
{
    Vec2 texBottomLeft(m_texMin.x, 1 - m_texMin.y);
    Vec2 texTopRight(m_texMax.x, 1 - m_texMax.y);
    m_buffers = createTextureRectBuffers(m_rect, texBottomLeft, texTopRight);
}

} }
