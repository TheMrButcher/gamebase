/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticFilledRect.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void StaticFilledRect::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
    TextureRect::loadResources();
}

void StaticFilledRect::serialize(Serializer& s) const
{
    s << "color" << m_color << "box" << m_box << "position" << m_offset;
}

std::unique_ptr<IObject> deserializeStaticFilledRect(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(GLColor, color);
    std::unique_ptr<StaticFilledRect> result(new StaticFilledRect(box, position));
    result->setColor(color);
    return std::move(result);
}

REGISTER_CLASS(StaticFilledRect);

} }
