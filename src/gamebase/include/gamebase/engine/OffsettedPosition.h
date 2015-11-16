#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/FixedOffset.h>

namespace gamebase {

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

}
