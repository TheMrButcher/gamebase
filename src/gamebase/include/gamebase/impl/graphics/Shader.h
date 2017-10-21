/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/graphics/typedefs.h>
#include <boost/noncopyable.hpp>
#include <string>

namespace gamebase { namespace impl {

class GAMEBASE_API Shader : boost::noncopyable {
public:
    Shader(GLenum type, const std::string& name)
        : m_type(type)
        , m_name(name)
        , m_id(0)
    {}

    const std::string& name() const { return m_name; }
    GLuint id() const { return m_id; }

    GLuint load();

private:
    GLenum m_type;
    std::string m_name;
    GLuint m_id;
};

} }
