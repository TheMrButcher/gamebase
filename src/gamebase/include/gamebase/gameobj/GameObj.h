/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/GameObjHelpers.h>

namespace gamebase {

class GameObj {
public:
    template <typename T> T skin() const;
    template <typename T> T child(const std::string& name) const;

    int id() const;
    void kill();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
	Vec2 move(float x, float y);
	Vec2 move(const Vec2& v);

    float scale() const;
    void setScale(float scale);
    float scaleX() const;
    void setScaleX(float scale);
    float scaleY() const;
    void setScaleY(float scale);

    float angle() const;
    void setAngle(float angle);
	float rotate(float angleDelta);

    void setSize(float width, float height);
    void setSize(const Vec2& size);

    void setCallback(const std::function<void()>& callback);

    bool isMouseOn() const;
    bool isPressed() const;
    bool isClicked() const;

    bool isEnabled() const;
    void setEnabled(bool value);
    void enable();
    void disable();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Box box() const;
    float width() const;
    float height() const;
	template <typename T> bool intersects(const T& obj) const;
	bool intersects(const Box& box) const;

    operator bool() const;

    struct Animation {
        void run(const std::string& name, int channel = 0);
        void play(const std::string& name, int channel = 0);

        void reset(int channel);
        void reset();

        void setSpeed(float speed, int channel);
        void setSpeed(float speed);
        float speed(int channel) const;
        float speed() const;

        void pause(int channel);
        void pause();
        void resume(int channel);
        void resume();
        bool isPaused(int channel) const;
        bool isPaused() const;
        bool isRunning(int channel) const;
        bool isRunning() const;
        bool isEmpty(int channel) const;
        bool isEmpty() const;
        
        GAMEBASE_DEFINE_GAME_OBJ_ANIMATION_PIMPL();
    };
    Animation anim;

    GAMEBASE_DEFINE_GAME_OBJ_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T GameObj::skin() const { return impl::wrap<T>(m_impl->skin()); }
template <typename T> inline T GameObj::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline Vec2 GameObj::move(float x, float y) { return move(Vec2(x, y)); }
inline Vec2 GameObj::move(const Vec2& v) { setPos(pos() + v); return pos(); }
inline int GameObj::id() const { return m_impl->id(); }
inline void GameObj::kill() { m_impl->kill(); }
inline float GameObj::scale() const { return m_impl->scale(); }
inline void GameObj::setScale(float scale) { m_impl->setScale(scale); }
inline float GameObj::scaleX() const { return m_impl->scaleX(); }
inline void GameObj::setScaleX(float scale) { m_impl->setScaleX(scale); }
inline float GameObj::scaleY() const { return m_impl->scaleY(); }
inline void GameObj::setScaleY(float scale) { m_impl->setScaleY(scale); }
inline float GameObj::angle() const { return m_impl->angle(); }
inline void GameObj::setAngle(float angle) { m_impl->setAngle(angle); }
inline float GameObj::rotate(float angleDelta)
{
	float newAngle = angle() + angleDelta;
	setAngle(newAngle);
	return newAngle;
}
inline void GameObj::setSize(float width, float height) { m_impl->setFixedBox(width, height); }
inline void GameObj::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }
inline void GameObj::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
inline bool GameObj::isMouseOn() const { return m_impl->isMouseOn(); }
inline bool GameObj::isPressed() const { return m_impl->isPressed(); }
inline bool GameObj::isClicked() const { return m_impl->isClicked(); }
template <typename T> inline bool GameObj::intersects(const T& obj) const { return intersects(obj.box()); }
inline bool GameObj::intersects(const Box& box) const { return this->box().intersects(box); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(GameObj);

inline void GameObj::Animation::run(const std::string& name, int channel) { m_obj->m_impl->runAnimation(name, channel); }
inline void GameObj::Animation::play(const std::string& name, int channel) { run(name, channel); }
inline void GameObj::Animation::reset(int channel) { m_obj->m_impl->resetChannel(channel); }
inline void GameObj::Animation::reset() { m_obj->m_impl->resetAllChannels(); }
inline void GameObj::Animation::setSpeed(float speed, int channel) { m_obj->m_impl->setChannelSpeed(channel, speed); }
inline void GameObj::Animation::setSpeed(float speed) { m_obj->m_impl->setAllChannelsSpeed(speed); }
inline float GameObj::Animation::speed(int channel) const { return m_obj->m_impl->channelSpeed(channel); }
inline float GameObj::Animation::speed() const { return m_obj->m_impl->defaultChannelSpeed(); }
inline void GameObj::Animation::pause(int channel) { m_obj->m_impl->pauseChannel(channel); }
inline void GameObj::Animation::pause() { m_obj->m_impl->pauseAllChannels(); }
inline void GameObj::Animation::resume(int channel) { m_obj->m_impl->resumeChannel(channel); }
inline void GameObj::Animation::resume() { m_obj->m_impl->resumeAllChannels(); }
inline bool GameObj::Animation::isPaused(int channel) const { return m_obj->m_impl->isChannelPaused(channel); }
inline bool GameObj::Animation::isPaused() const { return m_obj->m_impl->isAnimationPaused(); }
inline bool GameObj::Animation::isRunning(int channel) const { return m_obj->m_impl->isChannelRunning(channel); }
inline bool GameObj::Animation::isRunning() const { return m_obj->m_impl->isAnimationRunning(); }
inline bool GameObj::Animation::isEmpty(int channel) const { return m_obj->m_impl->isChannelEmpty(channel); }
inline bool GameObj::Animation::isEmpty() const { return m_obj->m_impl->isAnimationEmpty(); }

}
