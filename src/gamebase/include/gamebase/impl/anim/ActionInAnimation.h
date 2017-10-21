/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/anim/IAction.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API ActionInAnimation : public IAnimation, public ISerializable {
public:
    ActionInAnimation(const std::shared_ptr<IAction>& action)
        : m_action(action)
    {}

    virtual void load(const PropertiesRegister& props) override
    {
        m_action->load(props);
    }

    virtual void start() override
    {
        m_done = false;
    }

    virtual Time step(Time t) override
    {
        if (m_done)
            return t;
        m_done = true;
        m_action->exec();
        return t;
    }

    virtual bool isFinished() const override
    {
        return m_done;
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::shared_ptr<IAction> m_action;
    bool m_done;
};

} }
