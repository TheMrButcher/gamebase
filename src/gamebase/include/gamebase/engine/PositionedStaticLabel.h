#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/StaticLabel.h>

namespace gamebase {

class PositionedStaticLabel : public OffsettedPosition, public StaticLabel {
public:
    PositionedStaticLabel(const Vec2& position = Vec2())
        : OffsettedPosition(position)
        , StaticLabel(this)
    {}
};

}
