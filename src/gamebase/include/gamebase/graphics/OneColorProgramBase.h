#pragma once

#include <gamebase/graphics/ProgramBase.h>
#include <gamebase/graphics/Color.h>

namespace gamebase {

class GAMEBASE_API OneColorProgramBase : public ProgramBase {
public:
    OneColorProgramBase(
        const std::string& name,
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

public:
    mutable Color color;

protected:
    virtual void locateUniforms();
    virtual void loadUniforms() const;
    
private:
    GLuint m_colorUniformLocation;
};

}
