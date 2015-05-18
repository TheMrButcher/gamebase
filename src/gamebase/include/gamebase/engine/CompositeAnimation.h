#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/serial/ISerializable.h>
#include <vector>

namespace gamebase {

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

    virtual void load(const PropertiesRegister& props)
    {
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            (*it)->load(props);
    }

    virtual void start()
    {
        m_cur = m_animations.begin();
        startElement();
    }

    virtual void step()
    {
        while (!isFinished()) {
            (*m_cur)->step();
            if ((*m_cur)->isFinished()) {
                ++m_cur;
                startElement();
            } else {
                return;
            }   
        }
    }

    virtual bool isFinished() const
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

}
