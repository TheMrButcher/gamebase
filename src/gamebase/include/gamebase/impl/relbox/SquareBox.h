/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <algorithm>

namespace gamebase { namespace impl {

class SquareBox : public IRelativeBox, public ISerializable {
public:
    virtual BoundingBox count(const BoundingBox& parentBox) const override
    {
        auto minSize = std::min(parentBox.width(), parentBox.height());
        return BoundingBox(
            Vec2(-0.5f * minSize, -0.5f * minSize),
            Vec2(0.5f * minSize, 0.5f * minSize));
    }

    virtual void serialize(Serializer&) const override {}
};

} }
