#pragma once

#include <gamebase/engine/IAnimation.h>
#include <vector>

namespace gamebase {

class ParallelAnimation : public IAnimation {
public:
    ParallelAnimation() {}

    void add(const std::shared_ptr<IAnimation>& animation)
    {
        m_animations.push_back(animation);
    }

    virtual void load(const PropertiesRegister& props)
    {
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            (*it)->load(props);
    }

    virtual void start()
    {
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            (*it)->start();
        m_numOfFinished = 0;
    }

    virtual void step()
    {
        size_t numOfFinished = 0;
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it) {
            if ((*it)->isFinished()) {
                ++numOfFinished;
                continue;
            }
            (*it)->step();
        }
        m_numOfFinished = numOfFinished;
    }

    virtual bool isFinished() const
    {
        return m_numOfFinished == m_animations.size();
    }

private:
    std::vector<std::shared_ptr<IAnimation>> m_animations;
    size_t m_numOfFinished;
};

}
