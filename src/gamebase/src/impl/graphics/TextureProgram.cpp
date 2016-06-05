#include <stdafx.h>
#include <gamebase/impl/graphics/TextureProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

TextureProgram::TextureProgram()
    : OneColorProgramBase(
        "Texture",
        config().shadersPath + "Texture.vs",
        config().shadersPath + "Texture.fs")
{
    m_attrs.add("textureCoords", 2);
}

void TextureProgram::resetUniforms() const
{
    OneColorProgramBase::resetUniforms();
    texture = GLTexture();
}

void TextureProgram::loadUniforms() const
{
    OneColorProgramBase::loadUniforms();
    texture.bind();
}

const TextureProgram& textureProgram()
{
    const auto& program = state().programs.texture;
    program.activate();
    program.resetUniforms();
    return program;
}

} }
