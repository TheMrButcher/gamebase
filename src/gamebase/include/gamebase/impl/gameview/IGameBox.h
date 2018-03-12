/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <boost/optional.hpp>

namespace gamebase { namespace impl {

class IGameBox : public ISerializable {
public:
    virtual boost::optional<BoundingBox> box() const = 0;
    virtual void setViewBox(const BoundingBox& viewBox) = 0;
};

} }
