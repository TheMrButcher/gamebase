/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/ProgramBase.h>
#include <gamebase/impl/graphics/GLTexture.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ColoredTextureProgram : public ProgramBase {
public:
	ColoredTextureProgram();

	virtual void resetUniforms() const;

public:
	mutable GLTexture texture;

protected:
	virtual void loadUniforms() const;
};

GAMEBASE_API const ColoredTextureProgram& coloredTextureProgram();

} }
