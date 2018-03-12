/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FixedBox : public IRelativeBox, public ISerializable {
public:
    FixedBox(const BoundingBox& box)
    {
        m_box = box;
    }

    FixedBox(float width, float height)
    {
        m_box = BoundingBox(
            Vec2(-0.5f * width, -0.5f * height),
            Vec2(0.5f * width, 0.5f * height));
    }

    void checkInited() { m_inited = true; }

    virtual BoundingBox count(const BoundingBox&) const override { return m_box; }

    virtual void serialize(Serializer& s) const override;
};

} }
