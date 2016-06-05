#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include "TextureHelpers.h"
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/graphics/Image.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

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

} }
