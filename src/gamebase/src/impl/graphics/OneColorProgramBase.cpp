/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/OneColorProgramBase.h>

namespace gamebase { namespace impl {

OneColorProgramBase::OneColorProgramBase(
    const std::string& name,
    const std::string& vertexShaderName,
    const std::string& fragmentShaderName)
    : ProgramBase(name, vertexShaderName, fragmentShaderName)
{}

void OneColorProgramBase::resetUniforms() const
{
    ProgramBase::resetUniforms();
    color = Color();
}

void OneColorProgramBase::locateUniforms()
{
    ProgramBase::locateUniforms();
    m_colorUniformLocation = locateUniform("uColor");
}

void OneColorProgramBase::loadUniforms() const
{
    ProgramBase::loadUniforms();
    glUniform4f(m_colorUniformLocation, color.r, color.g, color.b, color.a);
}

} }
