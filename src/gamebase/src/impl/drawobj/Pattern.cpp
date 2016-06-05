#include <stdafx.h>
#include <gamebase/impl/drawobj/Pattern.h>
#include "TextureHelpers.h"
#include <gamebase/impl/graphics/PatternProgram.h>
#include <gamebase/impl/graphics/Image.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

namespace {
float toTexCoord(
    float coord,
    float period,
    unsigned int imgSize,
    GLTexture::WrapMode wrap)
{
    if (wrap == GLTexture::Clamp)
        return 1;
    if (period <= 0)
        period = static_cast<float>(imgSize);
    if (period <= 0)
        return 1;
    return coord / period;
}
}

Pattern::Pattern(
    const std::shared_ptr<IRelativeBox>& box,
    const std::shared_ptr<IRelativeOffset>& position)
    : StaticTextureRect(box, position)
    , m_wrapX(GLTexture::Repeat)
    , m_wrapY(GLTexture::Repeat)
{}

void Pattern::loadResources()
{
    if (imageName().empty())
        m_texture = loadPattern(DEFAULT_IMAGE_ID, m_wrapX, m_wrapY, &defaultImage);
    else
        m_texture = loadPattern(imageName(), m_wrapX, m_wrapY, std::bind(&loadImageFromFile, imageName()));
    m_texCoords.x = toTexCoord(box().width(), m_periods.x, m_texture.size().width, m_wrapX);
    m_texCoords.y = toTexCoord(box().height(), m_periods.y, m_texture.size().height, m_wrapY);
    m_buffers = createTextureRectBuffers(m_rect, Vec2(0, m_texCoords.y), Vec2(m_texCoords.x, 0));
}

void Pattern::drawAt(const Transform2& position) const
{
    if (m_color.a == 0)
        return;
    const PatternProgram& program = patternProgram();
    program.transform = position;
    program.texture = m_texture;
    program.color = m_color;
    program.texCoordsOffset = Vec2(m_texCoordsOffset.x, -m_texCoordsOffset.y);
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void Pattern::registerObject(PropertiesRegisterBuilder* builder)
{
    StaticTextureRect::registerObject(builder);
    builder->registerProperty("tv", &m_texCoordsOffset);
    builder->registerProperty("tx", &m_texCoordsOffset.x);
    builder->registerProperty("ty", &m_texCoordsOffset.y);
}

void Pattern::serialize(Serializer& s) const
{
    StaticTextureRect::serialize(s);
    s << "wrapX" << m_wrapX << "wrapY" << m_wrapY << "periods" << m_periods
        << "texCoordsOffset" << m_texCoordsOffset;
}

std::unique_ptr<IObject> deserializePattern(Deserializer& deserializer)
{
    DESERIALIZE(GLTexture::WrapMode, wrapX);
    DESERIALIZE(GLTexture::WrapMode, wrapY);
    DESERIALIZE(Vec2, periods);
    DESERIALIZE(Vec2, texCoordsOffset);
    auto result = deserializeTextureBase<Pattern>(deserializer);
    result->setWrapX(wrapX);
    result->setWrapY(wrapY);
    result->setPeriods(periods);
    result->setTexCoordsOffset(texCoordsOffset);
    return std::move(result);
}

REGISTER_CLASS(Pattern);

} }
