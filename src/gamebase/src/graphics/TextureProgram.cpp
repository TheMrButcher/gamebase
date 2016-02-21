#include <stdafx.h>
#include <gamebase/graphics/TextureProgram.h>
#include "State.h"
#include "src/core/Config.h"

namespace gamebase {

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
    texture = Texture();
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

}
