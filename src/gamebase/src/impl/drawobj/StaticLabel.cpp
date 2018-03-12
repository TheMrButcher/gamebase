/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/StaticLabel.h>
#include "src/impl/text/ITextRenderer.h"
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void StaticLabel::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;

    if (!m_parentBox.isValid())
        return;
    setBox(m_parentBox);
    loadResources();
}

void StaticLabel::registerObject(PropertiesRegisterBuilder* builder)
{
    std::function<void()> updater([this]()
    {
        if (m_renderer)
            m_renderer->setColor(m_color);
    });
    builder->registerProperty("color", &m_color, updater);
    builder->registerProperty("r", &m_color.r, updater);
    builder->registerProperty("g", &m_color.g, updater);
    builder->registerProperty("b", &m_color.b, updater);
    builder->registerProperty("a", &m_color.a, updater);
}

void StaticLabel::serialize(Serializer& s) const
{
    s   << "color" << m_color << "outlineColor" << m_outlineColor
        << "text" << m_text << "adjustSize" << m_adjustSize
        << "properties" << m_alignProps << "box" << m_box << "position" << m_offset;
}

std::unique_ptr<IObject> deserializeStaticLabel(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeOffset>, position);
    DESERIALIZE(GLColor, color);
    GLColor outlineColor;
    if (deserializer.hasMember("outlineColor"))
        deserializer >> "outlineColor" >> outlineColor;
    DESERIALIZE(AlignProperties, properties);
    DESERIALIZE(std::string, text);
    DESERIALIZE(bool, adjustSize);
    std::unique_ptr<StaticLabel> result(new StaticLabel(box, position));
    result->setColor(color);
    result->setOutlineColor(outlineColor);
    result->setAlignProperties(properties);
    result->setText(text);
    result->setAdjustSize(adjustSize);
    return std::move(result);
}

REGISTER_CLASS(StaticLabel);

} }
