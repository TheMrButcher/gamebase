/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/GLProgram.h>
#include <gamebase/math/Transform2.h>

namespace gamebase { namespace impl {

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

} }
