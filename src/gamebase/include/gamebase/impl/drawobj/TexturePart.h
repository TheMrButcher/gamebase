/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticTextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API TexturePart : public StaticTextureRect {
public:
    TexturePart(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const Vec2& texMin() const { return m_texMin; }
    void setTexMin(const Vec2& v) { m_texMin = v; update(); }

    const Vec2& texMax() const { return m_texMax; }
    void setTexMax(const Vec2& v) { m_texMax = v; update(); }

    void setTexRect(const Vec2& min, const Vec2& max)
    {
        m_texMin = min;
        m_texMax = max;
        update();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

private:
    virtual void updateImpl() override;

    Vec2 m_texMin;
    Vec2 m_texMax;
};

} }
