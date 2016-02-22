#pragma once

#include <gamebase/graphics/OneColorProgramBase.h>
#include <gamebase/graphics/GLTexture.h>

namespace gamebase {

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

}
