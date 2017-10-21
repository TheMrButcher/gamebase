/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/Skin.h>
#include <gamebase/impl/geom/IRelativeGeometry.h>

namespace gamebase { namespace impl {

class CheckBoxSkin : public Skin {
public:
    virtual std::shared_ptr<IRelativeGeometry> geometry() const = 0;
    virtual void setChecked(bool status) = 0;
};

} }
