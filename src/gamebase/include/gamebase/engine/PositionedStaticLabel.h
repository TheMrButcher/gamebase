#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/StaticLabel.h>

namespace gamebase {

class PositionedStaticLabel : public OffsettedPosition, public StaticLabel {
public:
    PositionedStaticLabel(
        const std::shared_ptr<IRelativeOffset>& offset)
        : OffsettedPosition(offset)
        , StaticLabel(this)
    {}

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        StaticLabel::setBox(allowedBox);
        m_offset->setBoxes(allowedBox, box());
    }
};

}
