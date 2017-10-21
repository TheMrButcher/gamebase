/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relbox/BoxSize.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ComplexBox : public IRelativeBox, public ISerializable {
public:
    ComplexBox()
        : m_widthSource(BoxSize::Width)
        , m_heightSource(BoxSize::Height)
        , m_horOffset(RelType::Pixels, 0)
        , m_vertOffset(RelType::Pixels, 0)
    {}

    ComplexBox(
        const RelativeValue& widthValue,
        const RelativeValue& heightValue,
        BoxSize::Type widthSource,
        BoxSize::Type heightSource,
        const RelativeValue& horOffset,
        const RelativeValue& vertOffset)
        : m_widthValue(widthValue)
        , m_heightValue(heightValue)
        , m_widthSource(widthSource)
        , m_heightSource(heightSource)
        , m_horOffset(horOffset)
        , m_vertOffset(vertOffset)
    {}

    virtual BoundingBox count(const BoundingBox& parentBox) const override
    {
        auto width = m_widthValue.count(size(parentBox, m_widthSource));
        auto height = m_heightValue.count(size(parentBox, m_heightSource));
        auto hoffset = m_horOffset.count(width);
        auto voffset = m_vertOffset.count(height);
        return BoundingBox(
            Vec2(-0.5f * width + hoffset, -0.5f * height + voffset),
            Vec2(0.5f * width + hoffset, 0.5f * height + voffset));
    }

	const RelativeValue& widthValue() const { return m_widthValue; };
    const RelativeValue& heightValue() const { return m_heightValue; };
    BoxSize::Type widthSource() const { return m_widthSource; };
    BoxSize::Type heightSource() const { return m_heightSource; };
    const RelativeValue& horOffset() const { return m_horOffset; };
    const RelativeValue& vertOffset() const { return m_vertOffset; };

    virtual void serialize(Serializer& s) const override;

private:
    float size(const BoundingBox& parentBox, BoxSize::Type type) const
    {
        switch (type) {
        case BoxSize::Width: return parentBox.width();
        case BoxSize::Height: return parentBox.height();
        case BoxSize::Min: return std::min(parentBox.width(), parentBox.height());
        case BoxSize::Max: return std::max(parentBox.width(), parentBox.height());
        default: THROW_EX() << "Unknown type of BoxSize" << static_cast<int>(type);
        }
    }

    RelativeValue m_widthValue;
    RelativeValue m_heightValue;
    BoxSize::Type m_widthSource;
    BoxSize::Type m_heightSource;
    RelativeValue m_horOffset;
    RelativeValue m_vertOffset;
};

} }
