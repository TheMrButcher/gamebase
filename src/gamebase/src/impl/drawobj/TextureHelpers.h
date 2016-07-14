/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

template <typename TexType>
std::unique_ptr<TexType> deserializeTextureBase(
    Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Color, color);
    DESERIALIZE(std::string, imageName);
    std::unique_ptr<TexType> result(new TexType(box, position));
    result->setColor(color);
    result->setImageName(imageName);
    return std::move(result);
}

} }
