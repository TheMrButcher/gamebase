#pragma once

#include <gamebase/engine/StaticTextureRect.h>

namespace gamebase {

class GAMEBASE_API Pattern : public StaticTextureRect {
public:
    Pattern(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    GLTexture::WrapMode wrapX() const { return m_wrapX; }
    void setWrapX(GLTexture::WrapMode value) { m_wrapX = value; }

    GLTexture::WrapMode wrapY() const { return m_wrapY; }
    void setWrapY(GLTexture::WrapMode value) { m_wrapY = value; }

    const Vec2& periods() const { return m_periods; }
    void setPeriods(const Vec2& v) { m_periods = v; }
    
    const Vec2& texCoordsOffset() const { return m_texCoordsOffset; }
    void setTexCoordsOffset(const Vec2& v) { m_texCoordsOffset = v; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    GLTexture::WrapMode m_wrapX;
    GLTexture::WrapMode m_wrapY;
    Vec2 m_periods;
    Vec2 m_texCoordsOffset;
    Vec2 m_texCoords;
};

}
