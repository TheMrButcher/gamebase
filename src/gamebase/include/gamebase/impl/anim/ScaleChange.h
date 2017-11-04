/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/ChangeFunc.h>
#include <gamebase/impl/anim/ChangeFuncPtr.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class InactiveObjectConstruct;

class ScaleChange : public IAnimation, public ISerializable {
public:
    enum Type {
        Both,
        X,
        Y
    };

    ScaleChange(
        const std::string& objName,
        Type scalingType,
        float scale,
        Time time,
        ChangeFunc::Type type,
        bool relativeScaling);

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
    Type m_scalingType;
    float m_scale;
    Time m_period;
    ChangeFunc::Type m_funcType;
    ChangeFuncPtr m_func;
    bool m_relativeScaling;

    Time m_cur;
    float m_curStartScaleX;
    float m_curDeltaScaleX;
    float m_curStartScaleY;
    float m_curDeltaScaleY;
};

} }
