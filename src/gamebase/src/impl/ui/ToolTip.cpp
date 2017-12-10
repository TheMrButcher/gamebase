/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/ui/ToolTip.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

namespace {
class ToolTipPosition : public IRelativeOffset {
public:
    ToolTipPosition(const std::shared_ptr<IRelativeOffset>& relativeOffset)
        : m_relativeOffset(relativeOffset)
        , m_positionType(ToolTip::RelativeToSourcePoint)
        , m_isJustShown(false)
    {}

    void onShowEvent() { m_isJustShown = true; }

    const std::shared_ptr<IRelativeOffset>& relativeOffset() const
    {
        return m_relativeOffset;
    }

    void setSourcePoint(const Vec2& sourcePoint)
    {
        switch (m_positionType) {
        case ToolTip::RelativeToSourcePoint: setAcceptedSourcePoint(sourcePoint); break;
        case ToolTip::RelativeToFirstSourcePoint:
            if (m_isJustShown) {
                setAcceptedSourcePoint(sourcePoint);
                m_isJustShown = false;
            }
            break;
        }
        m_sourcePoint = sourcePoint;
    }

    ToolTip::PositionType positionType() const
    {
        return m_positionType;
    }

    void setPositionType(ToolTip::PositionType value)
    {
        m_positionType = value;
    }

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const override
    {
        Vec2 result;
        if (m_relativeOffset)
            result = m_relativeOffset->count(parentBox, thisBox);
        result += m_acceptedSourcePoint;
        return result;
    }

private:
    void setAcceptedSourcePoint(const Vec2& sourcePoint)
    {
        m_pos.offset += sourcePoint - m_acceptedSourcePoint;
        m_acceptedSourcePoint = sourcePoint;
    }

    std::shared_ptr<IRelativeOffset> m_relativeOffset;
    ToolTip::PositionType m_positionType;
    Vec2 m_sourcePoint;
    Vec2 m_acceptedSourcePoint;
    bool m_isJustShown;
};
}

ToolTip::ToolTip(
    const std::shared_ptr<Drawable>& skin,
    const std::shared_ptr<IRelativeOffset>& position)
    : OffsettedPosition(std::make_shared<ToolTipPosition>(position))
    , Drawable(this)
    , m_skin(skin)
{}

void ToolTip::setSourcePoint(const Vec2& sourcePoint)
{
    static_cast<ToolTipPosition*>(m_offset.get())->setSourcePoint(sourcePoint);
}

ToolTip::PositionType ToolTip::positionType() const
{
    return static_cast<ToolTipPosition*>(m_offset.get())->positionType();
}

void ToolTip::setPositionType(PositionType value)
{
    static_cast<ToolTipPosition*>(m_offset.get())->setPositionType(value);
}

void ToolTip::setVisible(bool visible)
{
    if (visible && !isVisible())
        static_cast<ToolTipPosition*>(m_offset.get())->onShowEvent();
    Drawable::setVisible(visible);
}

void ToolTip::registerObject(PropertiesRegisterBuilder* builder)
{
    if (auto registrable = dynamic_cast<IRegistrable*>(m_skin.get()))
        builder->registerObject("skin", registrable);
}

void ToolTip::serialize(Serializer& s) const
{
    s << "positionType" << positionType()
        << "position" << static_cast<ToolTipPosition*>(m_offset.get())->relativeOffset()
        << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeToolTip(Deserializer& deserializer)
{
    DESERIALIZE(ToolTip::PositionType, positionType);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    auto result = std::make_unique<ToolTip>(skin, position);
    result->setPositionType(positionType);
    return result;
}

REGISTER_CLASS(ToolTip);

} }
