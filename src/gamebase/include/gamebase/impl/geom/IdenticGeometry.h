/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/IRelativeGeometry.h>
#include <gamebase/impl/geom/RectGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class IdenticGeometry : public IRelativeGeometry, public ISerializable {
public:
    virtual std::shared_ptr<IGeometry> count(const BoundingBox& box) const override
    {
        return std::make_shared<RectGeometry>(box);
    }

    virtual void serialize(Serializer&) const override {}
};

} }
