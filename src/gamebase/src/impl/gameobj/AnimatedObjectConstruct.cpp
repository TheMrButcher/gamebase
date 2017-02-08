/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/gameobj/AnimatedObjectConstruct.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/IDeserializer.h>
#include <gamebase/impl/serial/ISerializer.h>

namespace gamebase { namespace impl {

AnimatedObjectConstruct::AnimatedObjectConstruct(
    const std::shared_ptr<Drawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : InactiveObjectConstruct(drawable, position)
{}

void AnimatedObjectConstruct::serialize(Serializer& s) const
{
    InactiveObjectConstruct::serialize(s);
    s << "animations" << m_animations;
}

std::unique_ptr<IObject> deserializeAnimatedObjectConstruct(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<Drawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    DESERIALIZE(int, id);

    typedef std::map<std::string, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);

    std::unique_ptr<AnimatedObjectConstruct> result(new AnimatedObjectConstruct(skin, position));
    for (auto it = animations.begin(); it != animations.end(); ++it)
        result->addAnimation(it->first, it->second);
    result->setID(id);
    return std::move(result);
}

REGISTER_CLASS(AnimatedObjectConstruct);

} }
