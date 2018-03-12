/**
 * Copyright (c) 2018 Slavnejshev Filipp
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
    AligningOffset()
        : m_horAlign(HorAlign::Center)
        , m_vertAlign(VertAlign::Center)
        , m_horOffset(RelType::Pixels, 0.0f)
        , m_vertOffset(RelType::Pixels, 0.0f)
        , m_relativeToParent(true)
    {}

    AligningOffset(
        HorAlign::Enum horAlign,
        VertAlign::Enum vertAlign)
        : m_horAlign(horAlign)
        , m_vertAlign(vertAlign)
        , m_horOffset(RelType::Pixels, 0.0f)
        , m_vertOffset(RelType::Pixels, 0.0f)
        , m_relativeToParent(true)
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
        , m_relativeToParent(true)
    {}

    HorAlign::Enum horAlign() const { return m_horAlign; }
    VertAlign::Enum vertAlign() const { return m_vertAlign; }
    const RelativeValue& horOffset() const { return m_horOffset; }
    const RelativeValue& vertOffset() const { return m_vertOffset; }
    void setRelativeToParent(bool value) { m_relativeToParent = value; }

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const override
    {
        float width = m_relativeToParent ? parentBox.width() : thisBox.width();
        float height = m_relativeToParent ? parentBox.height() : thisBox.height();
        float horOffset = m_horOffset.count(width);
        float vertOffset = m_vertOffset.count(height);
        return Vec2(
            alignX(parentBox.bottomLeft.x, parentBox.topRight.x, thisBox.bottomLeft.x, thisBox.topRight.x) + horOffset,
            alignY(parentBox.bottomLeft.y, parentBox.topRight.y, thisBox.bottomLeft.y, thisBox.topRight.y) + vertOffset);
    }

    virtual void serialize(Serializer& s) const override;

private:
    float alignX(float parentLeftX, float parentRightX, float leftX, float rightX) const
    {
        switch (m_horAlign) {
            case HorAlign::Left: return parentLeftX - leftX;
            case HorAlign::Center: return 0.5f * (parentLeftX + parentRightX - leftX - rightX);
            case HorAlign::Right: return parentRightX - rightX;
            case HorAlign::None: return 0.0f;
            default: THROW_EX() << "Bad HorAlign::Enum value: " << static_cast<int>(m_horAlign);
        }
    }

    float alignY(float parentBottomY, float parentTopY, float bottomY, float topY) const
    {
        switch (m_vertAlign) {
            case VertAlign::Top: return parentTopY - topY;
            case VertAlign::Center: return 0.5f * (parentBottomY + parentTopY - bottomY - topY);
            case VertAlign::Bottom: return parentBottomY - bottomY;
            case VertAlign::None: return 0.0f;
            default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(m_vertAlign);
        }
    }

    HorAlign::Enum m_horAlign;
    VertAlign::Enum m_vertAlign;
    RelativeValue m_horOffset;
    RelativeValue m_vertOffset;
    bool m_relativeToParent;
};

} }
