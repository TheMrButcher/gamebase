/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/OneColorProgramBase.h>
#include <gamebase/impl/graphics/GLTexture.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TextureProgram : public OneColorProgramBase {
public:
    TextureProgram();

    virtual void resetUniforms() const;

public:
    mutable GLTexture texture;

protected:
    virtual void loadUniforms() const;
};

GAMEBASE_API const TextureProgram& textureProgram();

} }
