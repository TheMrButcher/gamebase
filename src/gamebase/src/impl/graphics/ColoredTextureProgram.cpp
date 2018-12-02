/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/ColoredTextureProgram.h>
#include "State.h"
#include "src/impl/global/Config.h"

namespace gamebase { namespace impl {

ColoredTextureProgram::ColoredTextureProgram()
	: ProgramBase(
		"ColoredTexture",
		config().shadersPath + "ColoredTexture.vs",
		config().shadersPath + "ColoredTexture.fs")
{
	m_attrs.add("textureCoords", 2);
	m_attrs.add("color", 4);
}

void ColoredTextureProgram::resetUniforms() const
{
	ProgramBase::resetUniforms();
	texture = GLTexture();
}

void ColoredTextureProgram::loadUniforms() const
{
	ProgramBase::loadUniforms();
	texture.bind();
}

const ColoredTextureProgram& coloredTextureProgram()
{
	const auto& program = state().programs.coloredTexture;
	program.activate();
	program.resetUniforms();
	return program;
}

} }
