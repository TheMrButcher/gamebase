/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relpos/IRelativeOffset.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API RelativeBox : public IRelativeBox, public ISerializable {
public:
    RelativeBox(
        RelativeValue widthValue,
        RelativeValue heightValue,
        const std::shared_ptr<IRelativeOffset>& offset = nullptr)
        : m_widthValue(widthValue)
        , m_heightValue(heightValue)
        , m_offset(offset)
    {}

    virtual BoundingBox count(const BoundingBox& parentBox) const override
    {
        auto width = m_widthValue.count(parentBox.width());
        auto height = m_heightValue.count(parentBox.height());
        BoundingBox result(
            Vec2(-0.5f * width, -0.5f * height),
            Vec2(0.5f * width, 0.5f * height));
        if (m_offset)
            result.move(m_offset->count(parentBox, result));
        return result;
    }

    virtual void serialize(Serializer& s) const override;

private:
    RelativeValue m_widthValue;
    RelativeValue m_heightValue;
    std::shared_ptr<IRelativeOffset> m_offset;
};

} }
