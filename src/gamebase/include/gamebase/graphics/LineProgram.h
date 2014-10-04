#pragma once

#include <gamebase/graphics/OneColorProgramBase.h>

namespace gamebase {

class GAMEBASE_API LineProgram : public OneColorProgramBase {
public:
    LineProgram();

protected:
    virtual void locateUniforms();
    virtual void loadUniforms() const;

private:
    GLuint m_extScaleLocation;
};

GAMEBASE_API const LineProgram& lineProgram();

}
