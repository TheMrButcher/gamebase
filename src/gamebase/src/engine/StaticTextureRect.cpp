#include <stdafx.h>
#include <gamebase/engine/StaticTextureRect.h>
#include "src/engine/TextureHelpers.h"
#include <gamebase/engine/FixedBox.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

void StaticTextureRect::setFixedBox(float width, float height)
{
    m_box = std::make_shared<FixedBox>(width, height);
}

GLTexture StaticTextureRect::loadTextureImpl(const std::string& imageName)
{
    if (imageName.empty())
        return loadTexture(DEFAULT_IMAGE_ID, &defaultImage);
    return loadTexture(imageName, std::bind(&loadImageFromFile, imageName));
}

void StaticTextureRect::loadResources()
{
    loadTextureImpl();
    TextureRect::loadResources();
}

void StaticTextureRect::serialize(Serializer& s) const
{
    s << "box" << m_box << "position" << m_offset << "color" << m_color << "imageName" << m_imageName;
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

}
