/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <vector>

namespace gamebase { namespace impl {

class GAMEBASE_API CompositeAnimation : public IAnimation, public ISerializable {
public:
    CompositeAnimation() {}

    CompositeAnimation(const std::vector<std::shared_ptr<IAnimation>>& animations)
        : m_animations(animations)
    {}

    void add(const std::shared_ptr<IAnimation>& animation)
    {
        m_animations.push_back(animation);
    }

    virtual void load(const PropertiesRegister& props) override
    {
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            (*it)->load(props);
    }

    virtual void start() override
    {
        m_cur = m_animations.begin();
        startElement();
    }

    virtual Time step(Time t) override
    {
        while (!isFinished()) {
            t = (*m_cur)->step(t);
            if ((*m_cur)->isFinished()) {
                ++m_cur;
                startElement();
            } else {
                return 0;
            }   
        }
        return t;
    }

    virtual bool isFinished() const override
    {
        return m_cur == m_animations.end();
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    void startElement()
    {
        if (!isFinished())
            (*m_cur)->start();
    }

    std::vector<std::shared_ptr<IAnimation>> m_animations;
    std::vector<std::shared_ptr<IAnimation>>::iterator m_cur;
};

} }
