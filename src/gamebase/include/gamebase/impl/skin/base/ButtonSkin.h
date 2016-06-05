#pragma once

#include <gamebase/impl/skin/base/Skin.h>
#include <gamebase/impl/geom/IRelativeGeometry.h>

namespace gamebase { namespace impl {

class ButtonSkin : public Skin {
public:
    virtual std::shared_ptr<IRelativeGeometry> geometry() const = 0;
};

} }
