#include <stdafx.h>
#include <gamebase/engine/TexturePart.h>
#include "src/engine/TextureHelpers.h"
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

TexturePart::TexturePart(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : StaticTextureRect(box, position)
    , m_texMin(0, 0)
    , m_texMax(1, 1)
{}

void TexturePart::loadResources()
{
    loadTextureImpl();
    update();
}

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

void TexturePart::update()
{
    if (isTextureLoaded()) {
        Vec2 texBottomLeft(m_texMin.x, 1 - m_texMin.y);
        Vec2 texTopRight(m_texMax.x, 1 - m_texMax.y);
        m_buffers = createTextureRectBuffers(m_rect, texBottomLeft, texTopRight);
    }
}

}
