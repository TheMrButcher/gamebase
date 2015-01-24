#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/engine/Direction.h>
#include <gamebase/geom/IRelativeGeometry.h>

namespace gamebase {

class ScrollDragBarSkin : public Skin {
public:
    virtual std::shared_ptr<IRelativeGeometry> geometry() const = 0;
    virtual Direction::Enum direction() const = 0;
};

}
