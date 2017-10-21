/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticLabel.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void StaticLabel::serialize(Serializer& s) const
{
    s << "color" << m_color << "text" << m_text << "adjustSize" << m_adjustSize
		 << "properties" << m_alignProps << "box" << m_box << "position" << m_offset;
}

std::unique_ptr<IObject> deserializeStaticLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(GLColor, color);
    DESERIALIZE(AlignProperties, properties);
    DESERIALIZE(std::string, text);
    DESERIALIZE(bool, adjustSize);
    std::unique_ptr<StaticLabel> result(new StaticLabel(box, position));
    result->setColor(color);
    result->setAlignProperties(properties);
    result->setText(text);
    result->setAdjustSize(adjustSize);
    return std::move(result);
}

REGISTER_CLASS(StaticLabel);

} }
