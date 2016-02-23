#pragma once

#include <gamebase/engine/StaticTextureRect.h>

namespace gamebase {

class GAMEBASE_API ComplexTexture : public StaticTextureRect {
public:
    ComplexTexture(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const Vec2& texCenter() const { return m_texCenter; }
    void setTexCenter(const Vec2& v) { m_texCenter = v; }

    const Vec2& angleSizes() const { return m_angleSizes; }
    void setAngleSizes(const Vec2& v) { m_angleSizes = v; }

    virtual void loadResources() override;
    virtual void serialize(Serializer& s) const override;

private:
    Vec2 m_texCenter;
    Vec2 m_angleSizes;
};

}
