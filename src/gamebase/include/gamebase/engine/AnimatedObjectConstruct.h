#pragma once

#include <gamebase/engine/InactiveObjectConstruct.h>
#include <gamebase/engine/MovableElement.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/AnimationManager.h>
#include <unordered_map>

namespace gamebase {

class GAMEBASE_API AnimatedObjectConstruct : public InactiveObjectConstruct, public IMovable {
public:
    AnimatedObjectConstruct(
        const std::shared_ptr<IDrawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);

    void addAnimation(const std::string& name, const std::shared_ptr<IAnimation>& anim)
    {
        m_animations[name] = anim;
    }

    void runAnimation(const std::string& name, int channel = 0)
    {
        auto it = m_animations.find(name);
        if (it == m_animations.end())
            THROW_EX() << "Can't find animation with name: " << name;
        m_animManager.addAnimation(it->second, channel);
    }

    void resetChannel(int channel) { m_animManager.resetChannel(channel); }
    void resetAllChannels() { m_animManager.reset(); }
    bool isChannelRunning(int channel) const { return m_animManager.isRunning(channel); }

    void setMover(const std::shared_ptr<MovableElement>& movElement)
    {
        m_movElement = movElement;
    }

    template <typename T>
    T* mover() const
    {
        return dynamic_cast<T*>(m_movElement.get());
    }

    virtual void loadResources() override
    {
        InactiveObjectConstruct::loadResources();
        for (auto it = m_animations.begin(); it != m_animations.end(); ++it)
            it->second->load(m_register);
        m_animManager.start();
    }

    virtual void move(float time) override
    {
        if (m_movElement)
            m_movElement->move(m_posElement.get(), time);
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& serializer) const override;

private:
    std::shared_ptr<MovableElement> m_movElement;
    std::unordered_map<std::string, std::shared_ptr<IAnimation>> m_animations;
    AnimationManager m_animManager;
};

}
