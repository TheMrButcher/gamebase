#include <stdafx.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/engine/FixedBox.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void StaticTextureRect::setFixedBox(float width, float height)
{
    m_box = std::make_shared<FixedBox>(width, height);
}

void StaticTextureRect::loadResources()
{
    if (m_imageName.empty())
        m_texture = loadTexture(DEFAULT_IMAGE_ID, &defaultImage);
    else
        m_texture = loadTexture(m_imageName, std::bind(&loadImageFromFile, m_imageName));
    TextureRect::loadResources();
}

void StaticTextureRect::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "color" << m_color << "imageName" << m_imageName;
}

std::unique_ptr<IObject> deserializeStaticTextureRect(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(Color, color);
    DESERIALIZE(std::string, imageName);
    std::unique_ptr<StaticTextureRect> result(new StaticTextureRect(box, position));
    result->setColor(color);
    result->setImageName(imageName);
    return std::move(result);
}

REGISTER_CLASS(StaticTextureRect);

}
