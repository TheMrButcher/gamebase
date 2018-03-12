/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/StaticTextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ComplexTexture : public StaticTextureRect {
public:
    ComplexTexture(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    const Vec2& texCenter() const { return m_texCenter; }
    void setTexCenter(const Vec2& v) { m_texCenter = v; update(); }

    const Vec2& angleSize() const { return m_angleSize; }
    void setAngleSize(const Vec2& v) { m_angleSize = v; update(); }

    virtual void serialize(Serializer& s) const override;

private:
    virtual void updateImpl() override;

    Vec2 m_texCenter;
    Vec2 m_angleSize;
};

} }
