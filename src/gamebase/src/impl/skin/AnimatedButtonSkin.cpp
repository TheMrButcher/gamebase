/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/AnimatedButtonSkin.h>
#include <gamebase/impl/relbox/FixedBox.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void AnimatedButtonSkin::setFixedBox(float width, float height)
{
    auto box = std::make_shared<FixedBox>(width, height);
    if (m_box->isValid())
        box->checkInited();
    m_box = box;
}

std::unique_ptr<IObject> deserializeAnimatedButtonSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    std::unique_ptr<AnimatedButtonSkin> result(new AnimatedButtonSkin(box, geometry));
    deserializeAnimatedObjectElements(deserializer, result.get());
    return std::move(result);
}

REGISTER_CLASS(AnimatedButtonSkin);

} }
