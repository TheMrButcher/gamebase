#pragma once

#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

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

}
