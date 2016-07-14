/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/anim/AnimationPause.h>
#include <gamebase/impl/anim/CompositeAnimation.h>
#include <gamebase/impl/anim/InstantChange.h>
#include <gamebase/impl/anim/ParallelAnimation.h>
#include <gamebase/impl/anim/RepeatingAnimation.h>
#include <gamebase/impl/anim/ActionInAnimation.h>
#include <gamebase/impl/anim/SmoothChange.h>
#include <gamebase/impl/anim/IncrementalChange.h>
#include <gamebase/impl/anim/FramesChange.h>
#include <gamebase/impl/drawobj/Atlas.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

void AnimationPause::serialize(Serializer& s) const
{
    s << "pauseTime" << m_period;
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
    Time t;
    if (deserializer.hasMember("time")) {
        DESERIALIZE(TypedTime, time);
        t = time.value;
    } else {
        DESERIALIZE(Time, pauseTime);
        t = pauseTime;
    }
    return std::unique_ptr<IObject>(new AnimationPause(t));
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
    Time t;
    if (deserializer.hasMember("time")) {
        DESERIALIZE(TypedTime, time);
        t = time.value;
    } else {
        DESERIALIZE(Time, period);
        t = period;
    }
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, moveToStart);
    std::unique_ptr<SmoothChange<T>> result(new SmoothChange<T>(
        propertyName, startValue, newValue, t, changeFunc));
    result->setMoveToStart(moveToStart);
    return std::move(result);
}

REGISTER_TEMPLATE(SmoothChange, float);
REGISTER_TEMPLATE(SmoothChange, double);
REGISTER_TEMPLATE(SmoothChange, int);
REGISTER_TEMPLATE(SmoothChange, Vec2);
REGISTER_TEMPLATE(SmoothChange, Color);

template <typename T>
std::unique_ptr<IObject> deserializeIncrementalChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, propertyName);
    DESERIALIZE(T, delta);
    DESERIALIZE(Time, period);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    return std::unique_ptr<IncrementalChange<T>>(new IncrementalChange<T>(
        propertyName, delta, period, changeFunc));
}

REGISTER_TEMPLATE(IncrementalChange, float);
REGISTER_TEMPLATE(IncrementalChange, Vec2);

void FramesChange::load(const PropertiesRegister& props)
{
    m_atlas = props.getObject<Atlas>(m_atlasName);
}

void FramesChange::start()
{
    if (!m_atlas)
        THROW_EX() << "Can't start animation, atlas " << m_atlasName << " is not loaded";
    auto maxFrameIndex = m_atlas->maxFrameIndex();
    if (m_lastFrameIndex < 0 || m_lastFrameIndex > maxFrameIndex)
        m_lastFrameIndex = maxFrameIndex;
    if (m_startFrameIndex < 0 || m_startFrameIndex > maxFrameIndex)
        m_startFrameIndex = maxFrameIndex;

    m_isPositiveDir = m_startFrameIndex <= m_lastFrameIndex;
    m_curTime = 0;
    m_curFrameIndex = m_startFrameIndex;
    m_curRepeat = 0;
    m_needUpdateFrame = true;
}

Time FramesChange::step(Time t)
{
    m_curTime += t;
    if (m_curTime > m_period) {
        if (m_isPositiveDir)
            ++m_curFrameIndex;
        else
            --m_curFrameIndex;
        bool isRepeatEnd = m_isPositiveDir
            ? m_curFrameIndex > m_lastFrameIndex
            : m_curFrameIndex < m_lastFrameIndex;
        if (isRepeatEnd) {
            m_curFrameIndex = m_startFrameIndex;
            m_curRepeat++;
        }
        if (isFinished()) {
            return m_curTime - m_period;
        } else {
            m_curTime -= m_period;
            m_needUpdateFrame = true;
        }
    }
    if (m_needUpdateFrame)
        m_atlas->setFrameIndex(m_curFrameIndex);
    return 0;
}

void FramesChange::serialize(Serializer& s) const
{
    s << "atlasName" << m_atlasName << "startFrameIndex" << m_startFrameIndex
        << "lastFrameIndex" << m_lastFrameIndex << "period" << m_period
        << "repeatTimes" << m_repeatTimes;
}

std::unique_ptr<IObject> deserializeFramesChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, atlasName);
    DESERIALIZE(int, startFrameIndex);
    DESERIALIZE(int, lastFrameIndex);
    DESERIALIZE(Time, period);
    DESERIALIZE(int, repeatTimes);
    return std::unique_ptr<FramesChange>(new FramesChange(
        atlasName, startFrameIndex, lastFrameIndex, period, repeatTimes));
}

REGISTER_CLASS(FramesChange);

} }
