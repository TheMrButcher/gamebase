#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IRelativeOffset.h>
#include <gamebase/engine/AlignEnums.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API AligningOffset : public IRelativeOffset, public ISerializable {
public:
    AligningOffset(
        HorAlign::Enum horAlign,
        VertAlign::Enum vertAlign)
        : m_horAlign(horAlign)
        , m_vertAlign(vertAlign)
    {}

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const override
    {
        return Vec2(
            countX(parentBox.bottomLeft.x, parentBox.topRight.x, thisBox.bottomLeft.x, thisBox.topRight.x),
            countY(parentBox.bottomLeft.y, parentBox.topRight.y, thisBox.bottomLeft.y, thisBox.topRight.y));
    }

    virtual void serialize(Serializer& s) const override;

private:
    float countX(float parentLeftX, float parentRightX, float leftX, float rightX) const
    {
        switch (m_horAlign) {
            case HorAlign::Left: return parentLeftX - leftX;
            case HorAlign::Center: return 0.5f * (parentLeftX + parentRightX - leftX - rightX);
            case HorAlign::Right: return parentRightX - rightX;
            default: THROW_EX() << "Bad HorAlign::Enum value: " << static_cast<int>(m_horAlign);
        }
    }

    float countY(float parentBottomY, float parentTopY, float bottomY, float topY) const
    {
        switch (m_vertAlign) {
            case VertAlign::Top: return parentTopY - topY;
            case VertAlign::Center: return 0.5f * (parentBottomY + parentTopY - bottomY - topY);
            case VertAlign::Bottom: return parentBottomY - bottomY;
            default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(m_vertAlign);
        }
    }

    HorAlign::Enum m_horAlign;
    VertAlign::Enum m_vertAlign;
};

}
