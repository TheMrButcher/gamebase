#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/geom/IRelativeGeometry.h>

namespace gamebase {

class ButtonSkin : public Skin {
public:
    virtual std::shared_ptr<IRelativeGeometry> geometry() const = 0;
};

}
