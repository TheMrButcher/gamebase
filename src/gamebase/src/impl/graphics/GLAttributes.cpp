/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/graphics/GLAttributes.h>
#include <gamebase/tools/Exception.h>

namespace gamebase { namespace impl {

void GLAttributes::locate(int programID, const std::string& programName)
{
    for (auto it = m_attrs.begin(); it != m_attrs.end(); ++it) {
        GLuint attrLocation = glGetAttribLocation(programID, it->name.c_str());
        if (attrLocation == 0xFFFFFFFF)
            THROW_EX() << "Can't locate attribute " << it->name << " in program " << programName;
        it->id = attrLocation;
    }
}

void GLAttributes::activate() const
{
    for (auto it = m_attrs.begin(); it != m_attrs.end(); ++it) {
        glEnableVertexAttribArray(it->id);
        glVertexAttribPointer(it->id, it->size, GL_FLOAT, GL_FALSE,
            sizeof(float) * m_size,
            reinterpret_cast<void*>(sizeof(float) * it->offset));
    }
}

void GLAttributes::disable() const
{
    for (auto it = m_attrs.begin(); it != m_attrs.end(); ++it)
        glDisableVertexAttribArray(it->id);
}

} }
