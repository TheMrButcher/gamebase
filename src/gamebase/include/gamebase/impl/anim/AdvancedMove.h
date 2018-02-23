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

struct Coord {
    enum Type {
        X,
        Y
    };
};

class InactiveObjectConstruct;

class AdvancedMove : public IAnimation, public ISerializable {
public:
    AdvancedMove(
        const std::string& objName,
        Coord::Type coordType,
        float pixelCoord,
        float parentWidthRelativeCoord,
        float parentHeightRelativeCoord,
        float selfWidthRelativeCoord,
        float selfHeightRelativeCoord,
        Time time,
        ChangeFunc::Type type,
        bool relativeMove);

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
    Coord::Type m_coordType;
    float m_pixelCoord;
    float m_parentWidthRelativeCoord;
    float m_parentHeightRelativeCoord;
    float m_selfWidthRelativeCoord;
    float m_selfHeightRelativeCoord;
    Time m_period;
    ChangeFunc::Type m_funcType;
    ChangeFuncPtr m_func;
    bool m_relativeMove;

    Time m_cur;
    float m_curStartValue;
    float m_curDeltaValue;
};

} }
