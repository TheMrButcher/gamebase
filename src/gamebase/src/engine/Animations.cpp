#include <stdafx.h>
#include <gamebase/engine/AnimationPause.h>
#include <gamebase/engine/CompositeAnimation.h>
#include <gamebase/engine/InstantChange.h>
#include <gamebase/engine/ParallelAnimation.h>
#include <gamebase/engine/RepeatingAnimation.h>
#include <gamebase/engine/SmoothChange.h>
#include <gamebase/serial/ISerializer.h>
#include <gamebase/serial/IDeserializer.h>

namespace gamebase {

void AnimationPause::serialize(Serializer& s) const
{
    s << "time" << TypedTime(m_timer.type(), m_period);
}

void CompositeAnimation::serialize(Serializer& s) const
{
    s << "animations" << m_animations;
}

void ParallelAnimation::serialize(Serializer& s) const
{
    s << "animations" << m_animations;
}

void RepeatingAnimation::serialize(Serializer& s) const
{
    s << "repeatTimes" << m_repeatTimes << "animation" << m_animation;
}

IObject* deserializeAnimationPause(Deserializer& deserializer)
{
    DESERIALIZE(TypedTime, time);
    return new AnimationPause(time);
}

IObject* deserializeCompositeAnimation(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<IAnimation>>, animations);
    return new CompositeAnimation(animations);
}

IObject* deserializeParallelAnimation(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<IAnimation>>, animations);
    return new ParallelAnimation(animations);
}

IObject* deserializeRepeatingAnimation(Deserializer& deserializer)
{
    DESERIALIZE(int, repeatTimes);
    DESERIALIZE(std::shared_ptr<IAnimation>, animation);
    return new RepeatingAnimation(repeatTimes, animation);
}

REGISTER_CLASS(AnimationPause);
REGISTER_CLASS(CompositeAnimation);
REGISTER_CLASS(ParallelAnimation);
REGISTER_CLASS(RepeatingAnimation);

template <typename T>
IObject* deserializeInstantChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, propertyName);
    DESERIALIZE(T, newValue);
    return new InstantChange<T>(propertyName, newValue);
}

REGISTER_TEMPLATE(InstantChange, float);
REGISTER_TEMPLATE(InstantChange, double);
REGISTER_TEMPLATE(InstantChange, int);
REGISTER_TEMPLATE(InstantChange, bool);
REGISTER_TEMPLATE(InstantChange, Vec2);
REGISTER_TEMPLATE(InstantChange, Color);
REGISTER_TEMPLATE(InstantChange, Matrix2);
REGISTER_TEMPLATE(InstantChange, Transform2);
REGISTER_TEMPLATE(InstantChange, BoundingBox);

template <typename T>
IObject* deserializeSmoothChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, propertyName);
    DESERIALIZE(T, startValue);
    DESERIALIZE(T, newValue);
    DESERIALIZE(TypedTime, time);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, moveToStart);
    auto* result = new SmoothChange<T>(
        propertyName, startValue, newValue, time, changeFunc);
    result->setMoveToStart(moveToStart);
    return result;
}

REGISTER_TEMPLATE(SmoothChange, float);
REGISTER_TEMPLATE(SmoothChange, double);
REGISTER_TEMPLATE(SmoothChange, int);
REGISTER_TEMPLATE(SmoothChange, Vec2);
REGISTER_TEMPLATE(SmoothChange, Color);

}
