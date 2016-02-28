#include <stdafx.h>
#include <gamebase/engine/AnimatedObjectConstruct.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/IDeserializer.h>
#include <gamebase/serial/ISerializer.h>

namespace gamebase {

AnimatedObjectConstruct::AnimatedObjectConstruct(
    const std::shared_ptr<IDrawable>& drawable,
    const std::shared_ptr<PositionElement>& position)
    : InactiveObjectConstruct(drawable, position)
{}

void AnimatedObjectConstruct::registerObject(PropertiesRegisterBuilder* builder)
{
    InactiveObjectConstruct::registerObject(builder);
    if (m_movElement)
        builder->registerObject(m_movElement.get());
}

void AnimatedObjectConstruct::serialize(Serializer& s) const
{
    InactiveObjectConstruct::serialize(s);
    if (dynamic_cast<ISerializable*>(m_movElement.get()) != nullptr) {
        s << "mover" << m_movElement;
    } else {
        std::shared_ptr<MovableElement> movable;
        s << "mover" << movable;
    }
    s << "animations" << m_animations;
}

std::unique_ptr<IObject> deserializeAnimatedObjectConstruct(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IDrawable>, skin);
    DESERIALIZE(std::shared_ptr<PositionElement>, position);
    DESERIALIZE(int, id);
    DESERIALIZE(std::shared_ptr<MovableElement>, mover);

    typedef std::map<std::string, std::shared_ptr<IAnimation>> Animations;
    DESERIALIZE(Animations, animations);

    std::unique_ptr<AnimatedObjectConstruct> result(new AnimatedObjectConstruct(skin, position));
    for (auto it = animations.begin(); it != animations.end(); ++it)
        result->addAnimation(it->first, it->second);
    result->setMover(mover);
    result->setID(id);
    return std::move(result);
}

REGISTER_CLASS(AnimatedObjectConstruct);

}
