/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/ChangeFunc.h>
#include <gamebase/impl/anim/ChangeFuncPtr.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class InactiveObjectConstruct;

class AngleChange : public IAnimation, public ISerializable {
public:
    AngleChange(
        const std::string& objName,
        float angle,
        Time time,
        ChangeFunc::Type type,
        bool relativeRotation);

    virtual void load(const PropertiesRegister& props) override;
    virtual void start() override;
    virtual Time step(Time t) override;

    virtual bool isFinished() const override
    {
        return m_cur >= m_period;
    }

    virtual void serialize(Serializer& s) const override;

private:
    std::string m_objName;
    InactiveObjectConstruct* m_obj;
    float m_angle;
    Time m_period;
    ChangeFunc::Type m_funcType;
    ChangeFuncPtr m_func;
    bool m_relativeRotation;

    Time m_cur;
    float m_curStartAngle;
    float m_curDeltaAngle;
};

} }
