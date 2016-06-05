#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <vector>

namespace gamebase { namespace impl {

class GAMEBASE_API ParallelAnimation : public IAnimation, public ISerializable {
public:
    ParallelAnimation() {}
    
    ParallelAnimation(const std::vector<std::shared_ptr<IAnimation>>& animations)
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
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            (*it)->start();
        m_numOfFinished = 0;
    }

    virtual Time step(Time t) override
    {
        size_t numOfFinished = 0;
        Time newT = t;
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it) {
            if ((*it)->isFinished()) {
                ++numOfFinished;
                continue;
            }
            newT = std::min(newT, (*it)->step(t));
        }
        m_numOfFinished = numOfFinished;
        return newT;
    }

    virtual bool isFinished() const override
    {
        return m_numOfFinished == m_animations.size();
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::vector<std::shared_ptr<IAnimation>> m_animations;
    size_t m_numOfFinished;
};

} }
