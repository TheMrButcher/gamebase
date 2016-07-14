/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/relpos/IRelativeOffset.h>
#include <gamebase/impl/relpos/AlignEnums.h>
#include <gamebase/impl/engine/RelativeValue.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API AligningOffset : public IRelativeOffset, public ISerializable {
public:
    AligningOffset(
        HorAlign::Enum horAlign,
        VertAlign::Enum vertAlign)
        : m_horAlign(horAlign)
        , m_vertAlign(vertAlign)
        , m_horOffset(RelType::Pixels, 0.0f)
        , m_vertOffset(RelType::Pixels, 0.0f)
    {}

    AligningOffset(
        HorAlign::Enum horAlign,
        VertAlign::Enum vertAlign,
        const RelativeValue& horOffset,
        const RelativeValue& vertOffset)
        : m_horAlign(horAlign)
        , m_vertAlign(vertAlign)
        , m_horOffset(horOffset)
        , m_vertOffset(vertOffset)
    {}

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const override
    {
        float horOffset = m_horOffset.count(parentBox.width());
        float vertOffset = m_vertOffset.count(parentBox.height());
        return Vec2(
            countX(parentBox.bottomLeft.x, parentBox.topRight.x, thisBox.bottomLeft.x, thisBox.topRight.x, horOffset),
            countY(parentBox.bottomLeft.y, parentBox.topRight.y, thisBox.bottomLeft.y, thisBox.topRight.y, vertOffset));
    }

    virtual void serialize(Serializer& s) const override;

private:
    float countX(float parentLeftX, float parentRightX, float leftX, float rightX, float offset) const
    {
        switch (m_horAlign) {
            case HorAlign::Left: return parentLeftX - leftX + offset;
            case HorAlign::Center: return 0.5f * (parentLeftX + parentRightX - leftX - rightX) + offset;
            case HorAlign::Right: return parentRightX - rightX + offset;
            default: THROW_EX() << "Bad HorAlign::Enum value: " << static_cast<int>(m_horAlign);
        }
    }

    float countY(float parentBottomY, float parentTopY, float bottomY, float topY, float offset) const
    {
        switch (m_vertAlign) {
            case VertAlign::Top: return parentTopY - topY + offset;
            case VertAlign::Center: return 0.5f * (parentBottomY + parentTopY - bottomY - topY) + offset;
            case VertAlign::Bottom: return parentBottomY - bottomY + offset;
            default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(m_vertAlign);
        }
    }

    HorAlign::Enum m_horAlign;
    VertAlign::Enum m_vertAlign;
    RelativeValue m_horOffset;
    RelativeValue m_vertOffset;
};

} }
