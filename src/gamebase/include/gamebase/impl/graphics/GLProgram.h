/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/Shader.h>
#include <gamebase/impl/graphics/VertexBuffer.h>
#include <gamebase/impl/graphics/IndexBuffer.h>
#include <gamebase/impl/graphics/GLAttributes.h>
#include <boost/noncopyable.hpp>

namespace gamebase { namespace impl {

class GAMEBASE_API GLProgram : boost::noncopyable {
public:
    GLProgram(
        const std::string& name,
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    const std::string& name() const { return m_name; }
    GLuint id() const { return m_id; }

    virtual GLuint load();
    virtual void activate() const;
    virtual void resetUniforms() const = 0;
    virtual void draw(const VertexBuffer& vbo, const IndexBuffer& ibo) const;

protected:
    GLAttributes m_attrs;

    GLuint locateUniform(const std::string& name) const;

private:
    virtual void locateUniforms() = 0;
    virtual void loadUniforms() const = 0;

    std::string m_name;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    GLuint m_id;
    bool m_loaded;
};

} }
