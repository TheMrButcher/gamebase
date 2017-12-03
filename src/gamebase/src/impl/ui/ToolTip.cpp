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
        , m_isRelativeToSourcePoint(true)
    {}

    const std::shared_ptr<IRelativeOffset>& relativeOffset() const
    {
        return m_relativeOffset;
    }

    void setSourcePoint(const Vec2& sourcePoint)
    {
        if (m_isRelativeToSourcePoint) {
            m_pos.offset += sourcePoint - m_sourcePoint;
        }
        m_sourcePoint = sourcePoint;
    }

    bool isRelativeToSourcePoint() const
    {
        return m_isRelativeToSourcePoint;
    }

    void setRelativeToSourcePoint(bool value)
    {
        if (m_isRelativeToSourcePoint == value)
            return;
        if (value)
            m_pos.offset += m_sourcePoint;
        else
            m_pos.offset -= m_sourcePoint;
    }

    virtual Vec2 count(
        const BoundingBox& parentBox, const BoundingBox& thisBox) const override
    {
        Vec2 result;
        if (m_relativeOffset)
            result = m_relativeOffset->count(parentBox, thisBox);
        if (m_isRelativeToSourcePoint)
            result += m_sourcePoint;
        return result;
    }

private:
    std::shared_ptr<IRelativeOffset> m_relativeOffset;
    bool m_isRelativeToSourcePoint;
    Vec2 m_sourcePoint;
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

bool ToolTip::isRelativeToSourcePoint() const
{
    return static_cast<ToolTipPosition*>(m_offset.get())->isRelativeToSourcePoint();
}

void ToolTip::setRelativeToSourcePoint(bool value)
{
    static_cast<ToolTipPosition*>(m_offset.get())->setRelativeToSourcePoint(value);
}

void ToolTip::registerObject(PropertiesRegisterBuilder* builder)
{
    if (auto registrable = dynamic_cast<IRegistrable*>(m_skin.get()))
        builder->registerObject("skin", registrable);
}

void ToolTip::serialize(Serializer& s) const
{
    s << "isRelativeToSourcePoint" << isRelativeToSourcePoint()
        << "position" << static_cast<ToolTipPosition*>(m_offset.get())->relativeOffset()
        << "skin" << m_skin;
}

std::unique_ptr<IObject> deserializeToolTip(Deserializer& deserializer)
{
    DESERIALIZE(bool, isRelativeToSourcePoint);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    auto result = std::make_unique<ToolTip>(skin, position);
    result->setRelativeToSourcePoint(isRelativeToSourcePoint);
    return result;
}

REGISTER_CLASS(ToolTip);

} }
