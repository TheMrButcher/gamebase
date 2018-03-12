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

struct ColorComponent {
    enum Type {
        R,
        G,
        B,
        A
    };
};

class ColorComponentChange : public IAnimation, public ISerializable {
public:
    ColorComponentChange(
        const std::string& objName,
        ColorComponent::Type colorComponentType,
        float colorComponent,
        Time time,
        ChangeFunc::Type type,
        bool relativeChange);

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
    std::shared_ptr<Value<float>> m_property;
    ColorComponent::Type m_colorComponentType;
    float m_colorComponent;
    Time m_period;
    ChangeFunc::Type m_funcType;
    ChangeFuncPtr m_func;
    bool m_relativeChange;

    Time m_cur;
    float m_curStartColorComponent;
    float m_curDeltaColorComponent;
};

} }
