/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API RepeatingAnimation : public IAnimation, public ISerializable {
public:
    static const int INFINITE_ANIMATION = -1;

    RepeatingAnimation(int repeatTimes, const std::shared_ptr<IAnimation>& animation)
        : m_repeatTimes(repeatTimes)
        , m_animation(animation)
    {}

    virtual void load(const PropertiesRegister& props) override
    {
        m_animation->load(props);
    }

    virtual void start() override
    {
        m_cur = 0;
        m_needRestart = true;
    }

    virtual Time step(Time t) override
    {
        static const int WARNING_COUNTER = 100;
        int c = 0;
        while (!isFinished()) {
            bool isJustStarted = false;
            if (m_needRestart) {
                m_animation->start();
                m_needRestart = false;
                isJustStarted = true;
            }
            auto newT = m_animation->step(t);
            if (isJustStarted && t == newT && m_repeatTimes == INFINITE_ANIMATION)
                ++c;
            if (c >= WARNING_COUNTER)
                THROW_EX() << "Detected infinite animation";
            t = newT;
            if (!m_animation->isFinished())
                break;
            ++m_cur;
            m_needRestart = true;
        }
        return t;
    }

    virtual bool isFinished() const override
    {
        return m_repeatTimes != INFINITE_ANIMATION && m_cur >= m_repeatTimes;
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    int m_repeatTimes;
    std::shared_ptr<IAnimation> m_animation;
    int m_cur;
    bool m_needRestart;
};

} }
