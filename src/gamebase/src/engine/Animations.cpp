#include <stdafx.h>
#include <gamebase/engine/AnimationPause.h>
#include <gamebase/engine/CompositeAnimation.h>
#include <gamebase/engine/InstantChange.h>
#include <gamebase/engine/ParallelAnimation.h>
#include <gamebase/engine/RepeatingAnimation.h>
#include <gamebase/engine/ActionInAnimation.h>
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

void ActionInAnimation::serialize(Serializer& s) const
{
    s << "action" << m_action;
}

std::unique_ptr<IObject> deserializeAnimationPause(Deserializer& deserializer)
{
    DESERIALIZE(TypedTime, time);
    return std::unique_ptr<IObject>(new AnimationPause(time));
}

std::unique_ptr<IObject> deserializeCompositeAnimation(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<IAnimation>>, animations);
    return std::unique_ptr<IObject>(new CompositeAnimation(animations));
}

std::unique_ptr<IObject> deserializeParallelAnimation(Deserializer& deserializer)
{
    DESERIALIZE(std::vector<std::shared_ptr<IAnimation>>, animations);
    return std::unique_ptr<IObject>(new ParallelAnimation(animations));
}

std::unique_ptr<IObject> deserializeRepeatingAnimation(Deserializer& deserializer)
{
    DESERIALIZE(int, repeatTimes);
    DESERIALIZE(std::shared_ptr<IAnimation>, animation);
    return std::unique_ptr<IObject>(new RepeatingAnimation(repeatTimes, animation));
}

std::unique_ptr<IObject> deserializeActionInAnimation(Deserializer& deserializer)
{
    DESERIALIZE(std::shared_ptr<IAction>, action);
    return std::unique_ptr<IObject>(new ActionInAnimation(action));
}

REGISTER_CLASS(AnimationPause);
REGISTER_CLASS(CompositeAnimation);
REGISTER_CLASS(ParallelAnimation);
REGISTER_CLASS(RepeatingAnimation);
REGISTER_CLASS(ActionInAnimation);

template <typename T>
std::unique_ptr<IObject> deserializeInstantChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, propertyName);
    DESERIALIZE(T, newValue);
    return std::unique_ptr<IObject>(new InstantChange<T>(propertyName, newValue));
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
std::unique_ptr<IObject> deserializeSmoothChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, propertyName);
    DESERIALIZE(T, startValue);
    DESERIALIZE(T, newValue);
    DESERIALIZE(TypedTime, time);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, moveToStart);
    std::unique_ptr<SmoothChange<T>> result(new SmoothChange<T>(
        propertyName, startValue, newValue, time, changeFunc));
    result->setMoveToStart(moveToStart);
    return std::move(result);
}

REGISTER_TEMPLATE(SmoothChange, float);
REGISTER_TEMPLATE(SmoothChange, double);
REGISTER_TEMPLATE(SmoothChange, int);
REGISTER_TEMPLATE(SmoothChange, Vec2);
REGISTER_TEMPLATE(SmoothChange, Color);

}
