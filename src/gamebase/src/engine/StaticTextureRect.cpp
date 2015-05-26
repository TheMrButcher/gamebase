#include <stdafx.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void StaticTextureRect::loadResources()
{
    std::unique_ptr<Image> image;
    if (m_imageName.empty())
        image = defaultImage();
    else
        image = loadImageFromFile(m_imageName);
    m_texture = Texture(*image);
    TextureRect::loadResources();
}

void StaticTextureRect::serialize(Serializer& s) const
{
    s << "box" << m_box << "color" << m_color << "imageName" << m_imageName;
}

std::unique_ptr<IObject> deserializeStaticTextureRect(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(Color, color);
    DESERIALIZE(std::string, imageName);
    std::unique_ptr<StaticTextureRect> result(new StaticTextureRect(box));
    result->setColor(color);
    result->setImageName(imageName);
    return std::move(result);
}

REGISTER_CLASS(StaticTextureRect);

}
