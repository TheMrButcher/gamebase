#include <stdafx.h>
#include <gamebase/impl/drawobj/Atlas.h>
#include "TextureHelpers.h"
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

Atlas::Atlas(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : Drawable(this)
    , OffsettedPosition(position)
    , m_texture(box)
    , m_frameIndex(0)
    , m_lineIndex(0)
    , m_frameSize(0, 0)
    , m_texFrameSize(0, 0)
{}

void Atlas::loadResources()
{
    auto texture = Texture::loadTextureImpl(imageName());
    m_texFrameSize.x = m_frameSize.x * (texture.size().width > 0 ? 1.0f / texture.size().width : 0);
    m_texFrameSize.y = m_frameSize.y * (texture.size().height > 0 ?  1.0f / texture.size().height : 0);
    m_maxFrameIndex = m_frameSize.x > 0
        ? static_cast<int>(texture.size().width / m_frameSize.x - 0.999f)
        : 0;
    updateImpl();
    m_texture.loadResources();
}

void Atlas::registerObject(PropertiesRegisterBuilder* builder)
{
    m_texture.registerProperties("", builder);

    std::function<void()> updater = std::bind(&Atlas::update, this);
    builder->registerProperty("n", &m_frameIndex, updater);
    builder->registerProperty("line", &m_lineIndex, updater);
}

void Atlas::serialize(Serializer& s) const
{
    s << "box" << m_texture.relativeBox() << "position" << m_offset
        << "color" << color() << "imageName" << imageName()
        << "frameIndex" << m_frameIndex << "lineIndex" << m_lineIndex
        << "frameSize" << m_frameSize;
}

std::unique_ptr<IObject> deserializeAtlas(Deserializer& deserializer)
{
    DESERIALIZE(int, frameIndex);
    DESERIALIZE(int, lineIndex);
    DESERIALIZE(Vec2, frameSize);
    auto result = deserializeTextureBase<Atlas>(deserializer);
    result->setFrameIndex(frameIndex);
    result->setLineIndex(lineIndex);
    result->setFrameSize(frameSize);
    return std::move(result);
}

REGISTER_CLASS(Atlas);

void Atlas::update()
{
    if (isTextureLoaded())
        updateImpl();
}

void Atlas::updateImpl()
{
    Vec2 texMin(m_frameIndex * m_texFrameSize.x, m_lineIndex * m_texFrameSize.y);
    Vec2 texMax = texMin + m_texFrameSize;
    m_texture.setTexRect(texMin, texMax);
}

} }
