#pragma once

#include <gamebase/graphics/GLProgram.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class GAMEBASE_API ProgramBase : public GLProgram {
public:
    ProgramBase(
        const std::string& name,
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    virtual void resetUniforms() const;

public:
    mutable Transform2 transform;

protected:
    virtual void locateUniforms();
    virtual void loadUniforms() const;
    
private:
    GLuint m_matrixUniformLocation;
    GLuint m_offsetUniformLocation;
};

}
