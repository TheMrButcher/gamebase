#pragma once

#include <gamebase/graphics/Shader.h>
#include <gamebase/graphics/VertexBuffer.h>
#include <gamebase/graphics/IndexBuffer.h>
#include <gamebase/graphics/GLAttributes.h>
#include <boost/noncopyable.hpp>

namespace gamebase {

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
};

}
