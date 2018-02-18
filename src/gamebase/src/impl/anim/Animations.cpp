/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "src/impl/global/GlobalTemporary.h"
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
#include <gamebase/impl/anim/ScaleChange.h>
#include <gamebase/impl/anim/AngleChange.h>
#include <gamebase/impl/anim/ColorComponentChange.h>
#include <gamebase/impl/anim/ColorChange.h>
#include <gamebase/impl/anim/StartSound.h>
#include <gamebase/impl/drawobj/Atlas.h>
#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

typedef GLColor Color;

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
    if (m_atlasName.empty()) {
        m_frameProperty = props.getProperty<int>("frame");
    } else {
        auto* obj = props.getObject<IRegistrable>(m_atlasName);
        m_frameProperty = obj->properties().getProperty<int>("frame");
    }
}

void FramesChange::start()
{
    if (!m_frameProperty)
        THROW_EX() << "Can't start animation, atlas " << m_atlasName << " is not loaded";
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
        m_frameProperty->set(m_curFrameIndex);
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

namespace {
InactiveObjectConstruct* findObject(
    const PropertiesRegister& props,
    const std::string& objName)
{
    if (objName.empty()) {
        auto obj = dynamic_cast<InactiveObjectConstruct*>(props.holder());
        if (!obj)
            THROW_EX() << "Animation holder is not game object, can't apply animation";
        return obj;
    }

    return props.getObject<InactiveObjectConstruct>(objName);
}
}

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
    , m_func(getChangeFuncPtr(type))
	, m_relativeMove(relativeMove)
{}

void AdvancedMove::load(const PropertiesRegister& props)
{
    m_obj = findObject(props, m_objName);
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
	part = m_func(part);
	float newCoord = m_curStartValue;
	newCoord += lerp(0.f, m_curDeltaValue, part);

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

ScaleChange::ScaleChange(
    const std::string& objName,
    Type scalingType,
    float scale,
    Time time,
    ChangeFunc::Type type,
    bool relativeScaling)
    : m_objName(objName)
    , m_scalingType(scalingType)
    , m_scale(scale)
    , m_period(time)
    , m_funcType(type)
    , m_func(getChangeFuncPtr(type))
    , m_relativeScaling(relativeScaling)
{}

void ScaleChange::load(const PropertiesRegister& props)
{
    m_obj = findObject(props, m_objName);
}

void ScaleChange::start()
{
    m_cur = 0;

    if (m_scalingType == X || m_scalingType == Both) {
        m_curStartScaleX = m_obj->scaleX();
        m_curDeltaScaleX = m_relativeScaling
            ? m_scale : m_scale - m_curStartScaleX;
    }

    if (m_scalingType == Y || m_scalingType == Both) {
        m_curStartScaleY = m_obj->scaleY();
        m_curDeltaScaleY = m_relativeScaling
            ? m_scale : m_scale - m_curStartScaleY;
    }
}

Time ScaleChange::step(Time t)
{
    m_cur += t;
    float part = m_period == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_period, 0.0f, 1.0f);
    part = m_func(part);

    auto scaleX = m_obj->scaleX();
    auto scaleY = m_obj->scaleY();
    if (m_scalingType == X || m_scalingType == Both)
        scaleX = m_curStartScaleX + lerp(0.f, m_curDeltaScaleX, part);
    if (m_scalingType == Y || m_scalingType == Both)
        scaleY = m_curStartScaleY + lerp(0.f, m_curDeltaScaleY, part);
    m_obj->setScale(scaleX, scaleY);

    return m_cur >= m_period ? m_cur - m_period : 0;
}

void ScaleChange::serialize(Serializer& s) const
{
    s << "objName" << m_objName << "scalingType" << m_scalingType << "scale" << m_scale
        << "period" << m_period << "changeFunc" << m_funcType << "relativeScaling" << m_relativeScaling;
}

std::unique_ptr<IObject> deserializeScaleChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, objName);
    DESERIALIZE(ScaleChange::Type, scalingType);
    DESERIALIZE(float, scale);
    DESERIALIZE(Time, period);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, relativeScaling);
    return std::make_unique<ScaleChange>(
        objName, scalingType, scale, period, changeFunc, relativeScaling);
}

REGISTER_CLASS(ScaleChange);

AngleChange::AngleChange(
    const std::string& objName,
    float angle,
    Time time,
    ChangeFunc::Type type,
    bool relativeRotation)
    : m_objName(objName)
    , m_angle(angle)
    , m_period(time)
    , m_funcType(type)
    , m_func(getChangeFuncPtr(type))
    , m_relativeRotation(relativeRotation)
{}

void AngleChange::load(const PropertiesRegister& props)
{
    m_obj = findObject(props, m_objName);
}

void AngleChange::start()
{
    m_cur = 0;
    m_curStartAngle = m_obj->angle();
    m_curDeltaAngle = m_relativeRotation
        ? m_angle : m_angle - m_curStartAngle;
}

Time AngleChange::step(Time t)
{
    m_cur += t;
    float part = m_period == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_period, 0.0f, 1.0f);
    part = m_func(part);

    float angle = m_curStartAngle + lerp(0.f, m_curDeltaAngle, part);
    m_obj->setAngle(angle);

    return m_cur >= m_period ? m_cur - m_period : 0;
}

void AngleChange::serialize(Serializer& s) const
{
    s << "objName" << m_objName << "angle" << m_angle
        << "period" << m_period << "changeFunc" << m_funcType << "relativeRotation" << m_relativeRotation;
}

std::unique_ptr<IObject> deserializeAngleChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, objName);
    DESERIALIZE(float, angle);
    DESERIALIZE(Time, period);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, relativeRotation);
    return std::make_unique<AngleChange>(
        objName, angle, period, changeFunc, relativeRotation);
}

REGISTER_CLASS(AngleChange);

ColorComponentChange::ColorComponentChange(
    const std::string& objName,
    ColorComponent::Type colorComponentType,
    float colorComponent,
    Time time,
    ChangeFunc::Type type,
    bool relativeChange)
    : m_objName(objName)
    , m_colorComponentType(colorComponentType)
    , m_colorComponent(colorComponent)
    , m_period(time)
    , m_funcType(type)
    , m_func(getChangeFuncPtr(type))
    , m_relativeChange(relativeChange)
{}

void ColorComponentChange::load(const PropertiesRegister& props)
{
    static const char* NAMES[] = { "r", "g", "b", "a" };
    if (m_objName.empty()) {
        m_property = props.getProperty<float>(NAMES[m_colorComponentType]);
    } else {
        auto* obj = props.getObject<IRegistrable>(m_objName);
        m_property = obj->properties().getProperty<float>(NAMES[m_colorComponentType]);
    }
}

void ColorComponentChange::start()
{
    m_cur = 0;
    m_curStartColorComponent = m_property->get();
    m_curDeltaColorComponent = m_relativeChange
        ? m_colorComponent : m_colorComponent - m_curStartColorComponent;
}

Time ColorComponentChange::step(Time t)
{
    m_cur += t;
    float part = m_period == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_period, 0.0f, 1.0f);
    part = m_func(part);

    float colorComponent = m_curStartColorComponent + lerp(0.f, m_curDeltaColorComponent, part);
    m_property->set(colorComponent);

    return m_cur >= m_period ? m_cur - m_period : 0;
}

void ColorComponentChange::serialize(Serializer& s) const
{
    s << "objName" << m_objName << "colorComponentType" << m_colorComponentType << "colorComponent" << m_colorComponent
        << "period" << m_period << "changeFunc" << m_funcType << "relativeChange" << m_relativeChange;
}

std::unique_ptr<IObject> deserializeColorComponentChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, objName);
    DESERIALIZE(ColorComponent::Type, colorComponentType);
    DESERIALIZE(float, colorComponent);
    DESERIALIZE(Time, period);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, relativeChange);
    return std::make_unique<ColorComponentChange>(
        objName, colorComponentType, colorComponent, period, changeFunc, relativeChange);
}

REGISTER_CLASS(ColorComponentChange);

ColorChange::ColorChange(
    const std::string& objName,
    const GLColor& color,
    Time time,
    ChangeFunc::Type type,
    bool relativeChange)
    : m_objName(objName)
    , m_color(color)
    , m_period(time)
    , m_funcType(type)
    , m_func(getChangeFuncPtr(type))
    , m_relativeChange(relativeChange)
{}

void ColorChange::load(const PropertiesRegister& props)
{
    static const char* NAME = "color";
    if (m_objName.empty()) {
        m_property = props.getProperty<GLColor>(NAME);
    } else {
        auto* obj = props.getObject<IRegistrable>(m_objName);
        m_property = obj->properties().getProperty<GLColor>(NAME);
    }
}

void ColorChange::start()
{
    m_cur = 0;
    m_curStartColor = m_property->get();
    m_curFinalColor = m_color;
    if (m_relativeChange) {
        m_curFinalColor.r += m_curStartColor.r;
        m_curFinalColor.g += m_curStartColor.g;
        m_curFinalColor.b += m_curStartColor.b;
        m_curFinalColor.a += m_curStartColor.a;
    }
}

Time ColorChange::step(Time t)
{
    m_cur += t;
    float part = m_period == 0 ? 1.f : clamp(static_cast<float>(m_cur) / m_period, 0.0f, 1.0f);
    part = m_func(part);

    GLColor color = lerp(m_curStartColor, m_curFinalColor, part);
    m_property->set(color);

    return m_cur >= m_period ? m_cur - m_period : 0;
}

void ColorChange::serialize(Serializer& s) const
{
    s << "objName" << m_objName << "color" << m_color
        << "period" << m_period << "changeFunc" << m_funcType << "relativeChange" << m_relativeChange;
}

std::unique_ptr<IObject> deserializeColorChange(Deserializer& deserializer)
{
    DESERIALIZE(std::string, objName);
    DESERIALIZE(GLColor, color);
    DESERIALIZE(Time, period);
    DESERIALIZE(ChangeFunc::Type, changeFunc);
    DESERIALIZE(bool, relativeChange);
    return std::make_unique<ColorChange>(
        objName, color, period, changeFunc, relativeChange);
}

REGISTER_CLASS(ColorChange);

Time StartSound::step(Time t)
{
    if (m_done)
        return t;
    m_done = true;
    g_temp.audioManager.addAudio(m_filePath, m_channel);
    return t;
}

void StartSound::serialize(Serializer& s) const
{
    s << "filePath" << m_filePath << "channel" << m_channel;
}

std::unique_ptr<IObject> deserializeStartSound(Deserializer& deserializer)
{
    DESERIALIZE(std::string, filePath);
    DESERIALIZE(int, channel);
    return std::make_unique<StartSound>(filePath, channel);
}

REGISTER_CLASS(StartSound);

} }
