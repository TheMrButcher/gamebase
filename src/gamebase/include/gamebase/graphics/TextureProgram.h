#pragma once

#include <gamebase/graphics/OneColorProgramBase.h>
#include <gamebase/graphics/Texture.h>

namespace gamebase {

class GAMEBASE_API TextureProgram : public OneColorProgramBase {
public:
    TextureProgram();

    virtual void resetUniforms() const;

public:
    mutable Texture texture;

protected:
    virtual void loadUniforms() const;
};

GAMEBASE_API const TextureProgram& textureProgram();

}
