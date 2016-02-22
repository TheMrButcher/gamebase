#pragma once

#include <gamebase/engine/StaticTextureRect.h>

namespace gamebase {

class GAMEBASE_API TexturePart : public StaticTextureRect {
public:
    TexturePart(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const Vec2& texMin() const { return m_texMin; }
    void setTexMin(const Vec2& v) { m_texMin = v; }

    const Vec2& texMax() const { return m_texMax; }
    void setTexMax(const Vec2& v) { m_texMax = v; }

    virtual void loadResources() override;
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    void update();

    Vec2 m_texMin;
    Vec2 m_texMax;
};

}
