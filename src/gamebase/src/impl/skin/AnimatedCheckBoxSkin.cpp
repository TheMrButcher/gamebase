/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/skin/impl/AnimatedCheckBoxSkin.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {
    
void AnimatedCheckBoxSkin::setChecked(bool status)
{
    m_animManager.resetChannel(1);
    if (status)
        m_animManager.addAnimation(m_checkAnimation, 1);
    else
        m_animManager.addAnimation(m_uncheckAnimation, 1);
}

void AnimatedCheckBoxSkin::loadResources()
{
    AnimatedObject::loadResources();
    if (m_checkAnimation)
        m_checkAnimation->load(m_register);
    if (m_uncheckAnimation)
        m_uncheckAnimation->load(m_register);
}
    
void AnimatedCheckBoxSkin::serialize(Serializer& s) const
{
    AnimatedObject::serialize(s);
    s << "checkAnimation" << m_checkAnimation << "uncheckAnimation" << m_uncheckAnimation;
}

std::unique_ptr<IObject> deserializeAnimatedCheckBoxSkin(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IRelativeBox>, box);
    DESERIALIZE(std::shared_ptr<IRelativeGeometry>, geometry);
    DESERIALIZE(std::shared_ptr<IAnimation>, checkAnimation);
    DESERIALIZE(std::shared_ptr<IAnimation>, uncheckAnimation);
    std::unique_ptr<AnimatedCheckBoxSkin> result(new AnimatedCheckBoxSkin(box, geometry));
    deserializeAnimatedObjectElements(deserializer, result.get());
    result->setCheckAnimation(checkAnimation);
    result->setUncheckAnimation(uncheckAnimation);
    return std::move(result);
}

REGISTER_CLASS(AnimatedCheckBoxSkin);

} }
