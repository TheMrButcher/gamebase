#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/IRelativeOffset.h>

namespace gamebase {

class OffsettedPosition : public IPositionable {
public:
    OffsettedPosition() {}

    OffsettedPosition(const std::shared_ptr<IRelativeOffset>& offset)
        : m_offset(offset)
    {}

    void setPosition(const std::shared_ptr<IRelativeOffset>& offset)
    {
        m_offset = offset;
    }

    template <typename OffsetType>
    OffsetType* offset() const
    {
        return dynamic_cast<OffsetType*>(m_offset.get());
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
