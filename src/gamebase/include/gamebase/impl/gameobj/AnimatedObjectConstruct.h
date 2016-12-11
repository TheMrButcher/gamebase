/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameobj/InactiveObjectConstruct.h>
#include <gamebase/impl/anim/AnimationManager.h>
#include <unordered_map>

namespace gamebase { namespace impl {

class GAMEBASE_API AnimatedObjectConstruct : public InactiveObjectConstruct {
public:
    AnimatedObjectConstruct(
        const std::shared_ptr<IDrawable>& drawable,
        const std::shared_ptr<PositionElement>& position = nullptr);

    const std::unordered_map<std::string, std::shared_ptr<IAnimation>>& animations() const { return m_animations; }
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
    bool isChannelEmpty(int channel) const { return m_animManager.isEmpty(channel); }

    void setChannelSpeed(int channel, float speed) { m_animManager.setSpeed(channel, speed); }
    void setAllChannelsSpeed(float speed) { m_animManager.setSpeed(speed); }
    float channelSpeed(int channel) const { return m_animManager.speed(channel); }
    float defaultChannelSpeed() const { return m_animManager.speed(); }

    void pauseChannel(int channel) { m_animManager.pause(channel); }
    void pauseAllChannels() { m_animManager.pause(); }
    void resumeChannel(int channel) { m_animManager.resume(channel); }
    void resumeAllChannels() { m_animManager.resume(); }
    bool isChannelPaused(int channel) const { return m_animManager.isPaused(channel); }
    bool isPaused() const { return m_animManager.isPaused(); }

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

    virtual void serialize(Serializer& serializer) const override;

private:
    std::unordered_map<std::string, std::shared_ptr<IAnimation>> m_animations;
    AnimationManager m_animManager;
};

typedef AnimatedObjectConstruct AnimGameObj;

} }
