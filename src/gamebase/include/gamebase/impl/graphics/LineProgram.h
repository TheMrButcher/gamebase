/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/OneColorProgramBase.h>

namespace gamebase { namespace impl {

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

} }
