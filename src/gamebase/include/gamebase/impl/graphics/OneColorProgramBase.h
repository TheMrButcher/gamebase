/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/ProgramBase.h>
#include <gamebase/impl/graphics/GLColor.h>

namespace gamebase { namespace impl {

class GAMEBASE_API OneColorProgramBase : public ProgramBase {
public:
    OneColorProgramBase(
        const std::string& name,
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    virtual void resetUniforms() const;

public:
    mutable GLColor color;

protected:
    virtual void locateUniforms();
    virtual void loadUniforms() const;
    
private:
    GLuint m_colorUniformLocation;
};

} }
