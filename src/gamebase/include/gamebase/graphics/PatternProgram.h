#pragma once

#include <gamebase/graphics/OneColorProgramBase.h>
#include <gamebase/graphics/Texture.h>

namespace gamebase {

class GAMEBASE_API PatternProgram : public OneColorProgramBase {
public:
    PatternProgram();

    virtual void resetUniforms() const override;

public:
    mutable Texture texture;
    mutable Vec2 texCoordsOffset;

protected:
    virtual void locateUniforms() override;
    virtual void loadUniforms() const override;

private:
    GLuint m_texCoordsOffsetUniformLocation;
};

GAMEBASE_API const PatternProgram& patternProgram();

}
