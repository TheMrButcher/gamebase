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
#include <gamebase/impl/anim/InstantHide.h>
#include <gamebase/impl/anim/InstantShow.h>
#include <gamebase/impl/anim/AdvancedMove.h>
#include <gamebase/impl/drawobj/Atlas.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
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

void InstantVisibilityChange::load(const PropertiesRegister& props)
{
	if (m_objName.empty()) {
		m_drawable = dynamic_cast<IDrawable*>(props.holder());
		if (!m_drawable)
			THROW_EX() << "Animation holder is not drawable, can't change visibility";
		return;
	}

	m_drawable = props.getObject<IDrawable>(m_objName);
}

Time InstantVisibilityChange::step(Time t)
{
	if (m_done)
		return t;
	m_done = true;
	m_drawable->setVisible(m_value);
	return t;
}

void InstantHide::serialize(Serializer& s) const
{
	s << "objName" << m_objName;
}

void InstantShow::serialize(Serializer& s) const
{
	s << "objName" << m_objName;
}

std::unique_ptr<IObject> deserializeInstantHide(Deserializer& deserializer)
{
	DESERIALIZE(std::string, objName);
	return std::make_unique<InstantHide>(objName);
}

std::unique_ptr<IObject> deserializeInstantShow(Deserializer& deserializer)
{
	DESERIALIZE(std::string, objName);
	return std::make_unique<InstantShow>(objName);
}

REGISTER_CLASS(InstantHide);
REGISTER_CLASS(InstantShow);

AdvancedMove::AdvancedMove(
	const std::string& objName,
	Coord::Type coordType,
	float pixelCoord,
	float parentWidthRelativeCoord,
	float parentHeightRelativeCoord,
	float selfWidthRelativeCoord,
	float selfHeightRelativeCoord,
	Time time,
	ChangeFunc::Type type,
	bool relativeMove)
	: m_objName(objName)
	, m_coordType(coordType)
	, m_pixelCoord(pixelCoord)
	, m_parentWidthRelativeCoord(parentWidthRelativeCoord)
	, m_parentHeightRelativeCoord(parentHeightRelativeCoord)
	, m_selfWidthRelativeCoord(selfWidthRelativeCoord)
	, m_selfHeightRelativeCoord(selfHeightRelativeCoord)
	, m_period(time)
	, m_funcType(type)
	, m_relativeMove(relativeMove)
{}

void AdvancedMove::load(const PropertiesRegister& props)
{
	if (m_objName.empty()) {
		m_obj = dynamic_cast<InactiveObjectConstruct*>(props.holder());
		if (!m_obj)
			THROW_EX() << "Animation holder is not game object, can't apply advanced move";
		return;
	}

	m_obj = props.getObject<InactiveObjectConstruct>(m_objName);
}

void AdvancedMove::start()
{
	m_cur = 0;
	auto pos = m_obj->getOffset();
	m_curStartValue = m_coordType == Coord::X ? pos.x : pos.y;
	m_curDeltaValue = m_pixelCoord;
	if (!m_relativeMove)
		m_curDeltaValue -= m_curStartValue;
	if (m_parentWidthRelativeCoord != 0 || m_parentHeightRelativeCoord) {
		auto box = m_obj->parentBox();
		m_curDeltaValue += (box.width() * m_parentWidthRelativeCoord + box.height() * m_parentHeightRelativeCoord) / 100.f;
	}
	if (m_selfWidthRelativeCoord != 0 || m_selfHeightRelativeCoord) {
		auto box = m_obj->box();
		m_curDeltaValue += (box.width() * m_selfWidthRelativeCoord + box.height() * m_selfHeightRelativeCoord) / 100.f;
	}
}

Time AdvancedMove::step(Time t)
{
	m_cur += t;
	float part = m_period == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_period, 0.0f, 1.0f);
	float newCoord = m_curStartValue;
	switch (m_funcType) {
	case ChangeFunc::Linear: newCoord += lerp(0.f, m_curDeltaValue, part); break;
	}

	auto pos = m_obj->getOffset();
	if (m_coordType == Coord::X)
		pos.x = newCoord;
	else
		pos.y = newCoord;
	m_obj->setOffset(pos);

	return m_cur >= m_period ? m_cur - m_period : 0;
}

void AdvancedMove::serialize(Serializer& s) const
{
	s << "objName" << m_objName << "coordType" << m_coordType << "pixelCoord" << m_pixelCoord
		<< "parentWidthRelativeCoord" << m_parentWidthRelativeCoord
		<< "parentHeightRelativeCoord" << m_parentHeightRelativeCoord
		<< "selfWidthRelativeCoord" << m_selfWidthRelativeCoord
		<< "selfHeightRelativeCoord" << m_selfHeightRelativeCoord
		<< "period" << m_period << "changeFunc" << m_funcType << "relativeMove" << m_relativeMove;
}

std::unique_ptr<IObject> deserializeAdvancedMove(Deserializer& deserializer)
{
	DESERIALIZE(std::string, objName);
	DESERIALIZE(Coord::Type, coordType);
	DESERIALIZE(float, pixelCoord);
	DESERIALIZE(float, parentWidthRelativeCoord);
	DESERIALIZE(float, parentHeightRelativeCoord);
	DESERIALIZE(float, selfWidthRelativeCoord);
	DESERIALIZE(float, selfHeightRelativeCoord);
	DESERIALIZE(Time, period);
	DESERIALIZE(ChangeFunc::Type, changeFunc);
	DESERIALIZE(bool, relativeMove);
	return std::make_unique<AdvancedMove>(
		objName, coordType, pixelCoord, parentWidthRelativeCoord, parentHeightRelativeCoord,
		selfWidthRelativeCoord, selfHeightRelativeCoord, period, changeFunc, relativeMove);
}

REGISTER_CLASS(AdvancedMove);

} }
