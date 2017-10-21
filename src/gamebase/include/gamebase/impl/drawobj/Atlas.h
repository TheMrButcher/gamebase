/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/TexturePart.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Atlas : public Drawable, public OffsettedPosition,
    public Registrable, public ISerializable {
public:
    Atlas(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    bool isTextureLoaded() const { return m_texture.isTextureLoaded(); }
    
    const std::string& imageName() const { return m_texture.imageName(); }
    void setImageName(const std::string& name) { m_texture.setImageName(name); update(); }

    const GLColor& color() const { return m_texture.color(); }
    void setColor(const GLColor& color) { m_texture.setColor(color); }

    int frameIndex() const { return m_frameIndex; }
    void setFrameIndex(int value) { m_frameIndex = value; update(); }

    int maxFrameIndex() const { return m_maxFrameIndex; }

    int lineIndex() const { return m_lineIndex; }
    void setLineIndex(int value) { m_lineIndex = value; update(); }

    const Vec2& frameSize() const { return m_frameSize; }
    void setFrameSize(const Vec2& v) { m_frameSize = v; update(); }

    void setFixedBox(float width, float height) { m_texture.setFixedBox(width, height); }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override { m_texture.draw(position); }
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_texture.setBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }
    virtual BoundingBox box() const override { return m_texture.box(); }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    void update();
    void updateImpl();

    TexturePart m_texture;
    int m_frameIndex;
    int m_lineIndex;
    Vec2 m_frameSize;
    Vec2 m_texFrameSize;
    int m_maxFrameIndex;
};

} }
