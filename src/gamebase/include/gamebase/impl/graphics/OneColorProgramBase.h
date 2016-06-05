#pragma once

#include <gamebase/impl/graphics/ProgramBase.h>
#include <gamebase/graphics/Color.h>

namespace gamebase { namespace impl {

class GAMEBASE_API OneColorProgramBase : public ProgramBase {
public:
    OneColorProgramBase(
        const std::string& name,
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    virtual void resetUniforms() const;

public:
    mutable Color color;

protected:
    virtual void locateUniforms();
    virtual void loadUniforms() const;
    
private:
    GLuint m_colorUniformLocation;
};

} }
