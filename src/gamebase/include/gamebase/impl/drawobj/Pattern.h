/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticTextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Pattern : public StaticTextureRect {
public:
    Pattern(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    GLTexture::WrapMode wrapX() const { return m_wrapX; }
    void setWrapX(GLTexture::WrapMode value) { m_wrapX = value; update(); }

    GLTexture::WrapMode wrapY() const { return m_wrapY; }
    void setWrapY(GLTexture::WrapMode value) { m_wrapY = value; update(); }

    const Vec2& periods() const { return m_periods; }
    void setPeriods(const Vec2& v) { m_periods = v; update(); }
    
    const Vec2& texCoordsOffset() const { return m_texCoordsOffset; }
    void setTexCoordsOffset(const Vec2& v) { m_texCoordsOffset = v; update(); }

    virtual void drawAt(const Transform2& position) const override;
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    virtual void reloadImpl() override;
    virtual void updateImpl() override;

    GLTexture::WrapMode m_wrapX;
    GLTexture::WrapMode m_wrapY;
    Vec2 m_periods;
    Vec2 m_texCoordsOffset;
    Vec2 m_texCoords;
};

} }
