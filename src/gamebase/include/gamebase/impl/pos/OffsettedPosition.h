/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <memory>

namespace gamebase { namespace impl {

class OffsettedPosition : public IPositionable {
public:
    OffsettedPosition() {}

    OffsettedPosition(const std::shared_ptr<IRelativeOffset>& offset)
        : m_offset(offset)
    {}

    void setRelativeOffset(const std::shared_ptr<IRelativeOffset>& offset)
    {
        m_offset = offset;
    }

    template <typename OffsetType>
    OffsetType* offset() const
    {
        return dynamic_cast<OffsetType*>(m_offset.get());
    }

    Vec2 getOffset() const { return position().offset; }
    void setOffset(const Vec2& v)
    {
        auto* fixedOffsetRawPtr = dynamic_cast<FixedOffset*>(m_offset.get());
        if (!fixedOffsetRawPtr) {
            auto fixedOffset = std::make_shared<FixedOffset>(v);
            setRelativeOffset(fixedOffset);
            return;
        }
        fixedOffsetRawPtr->set(v);
    }
    void setOffset(float x, float y) { setOffset(Vec2(x, y)); }

    virtual Transform2 position() const override
    {
        return m_offset ? m_offset->get() : Transform2();
    }

protected:
    void setPositionBoxes(
        const BoundingBox& parentBox, const BoundingBox& thisBox)
    {
        if (!m_offset)
            return;
        m_offset->setBoxes(parentBox, thisBox);
    }

    std::shared_ptr<IRelativeOffset> m_offset;
};

} }
