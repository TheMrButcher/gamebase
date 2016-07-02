#pragma once

#include <gamebase/impl/pubhelp/GameObjHelpers.h>

namespace gamebase {

class GameObj {
public:
    template <typename T> T skin() const;
    template <typename T> T child(const std::string& name) const;

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);

    float scale() const;
    void setScale(float scale);
    float scaleX() const;
    void setScaleX(float scale);
    float scaleY() const;
    void setScaleY(float scale);

    float angle() const;
    void setAngle(float angle);

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

    BoundingBox box() const;
    BoundingBox movedBox() const;
    BoundingBox fullBox() const;

    struct Animation {
        void run(const std::string& name, int channel = 0);

        void reset(int channel);
        void reset();
        bool isRunning(int channel) const;
        bool isEmpty(int channel) const;

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
        
        GAMEBASE_DEFINE_GAME_OBJ_ANIMATION_PIMPL();
    };
    Animation anim;

    GAMEBASE_DEFINE_GAME_OBJ_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T GameObj::skin() const { return impl::wrap<T>(m_impl->skin()); }
template <typename T> inline T GameObj::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline float GameObj::scale() const { return m_impl->scale(); }
inline void GameObj::setScale(float scale) { m_impl->setScale(scale); }
inline float GameObj::scaleX() const { return m_impl->scaleX(); }
inline void GameObj::setScaleX(float scale) { m_impl->setScaleX(scale); }
inline float GameObj::scaleY() const { return m_impl->scaleY(); }
inline void GameObj::setScaleY(float scale) { m_impl->setScaleY(scale); }
inline float GameObj::angle() const { return m_impl->angle(); }
inline void GameObj::setAngle(float angle) { m_impl->setAngle(angle); }
inline void GameObj::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
inline bool GameObj::isMouseOn() const { return m_impl->isMouseOn(); }
inline bool GameObj::isPressed() const { return m_impl->isPressed(); }
inline bool GameObj::isClicked() const { return m_impl->isClicked(); }
inline BoundingBox GameObj::fullBox() const { return m_impl->fullBox(); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(GameObj);

inline void GameObj::Animation::run(const std::string& name, int channel) { m_obj->m_impl->runAnimation(name, channel); }
inline void GameObj::Animation::reset(int channel) { m_obj->m_impl->resetChannel(channel); }
inline void GameObj::Animation::reset() { m_obj->m_impl->resetAllChannels(); }
inline bool GameObj::Animation::isRunning(int channel) const { return m_obj->m_impl->isChannelRunning(channel); }
inline bool GameObj::Animation::isEmpty(int channel) const { return m_obj->m_impl->isChannelEmpty(channel); }
inline void GameObj::Animation::setSpeed(float speed, int channel) { m_obj->m_impl->setChannelSpeed(channel, speed); }
inline void GameObj::Animation::setSpeed(float speed) { m_obj->m_impl->setAllChannelsSpeed(speed); }
inline float GameObj::Animation::speed(int channel) const { return m_obj->m_impl->channelSpeed(channel); }
inline float GameObj::Animation::speed() const { return m_obj->m_impl->defaultChannelSpeed(); }
inline void GameObj::Animation::pause(int channel) { m_obj->m_impl->pauseChannel(channel); }
inline void GameObj::Animation::pause() { m_obj->m_impl->pauseAllChannels(); }
inline void GameObj::Animation::resume(int channel) { m_obj->m_impl->resumeChannel(channel); }
inline void GameObj::Animation::resume() { m_obj->m_impl->resumeAllChannels(); }
inline bool GameObj::Animation::isPaused(int channel) const { return m_obj->m_impl->isChannelPaused(channel); }
inline bool GameObj::Animation::isPaused() const { return m_obj->m_impl->isPaused(); }

}
