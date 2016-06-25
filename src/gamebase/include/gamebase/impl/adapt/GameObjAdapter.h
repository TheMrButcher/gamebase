#pragma once

#include <gamebase/impl/pubhelp/SmartPointer.h>
#include <gamebase/impl/gameobj/ObjectConstruct.h>

namespace gamebase { namespace impl {

class GameObjAdapter {
public:
    GameObjAdapter(const SmartPointer<InactiveObjectConstruct>& obj)
        : m_obj(obj)
        , m_anim(nullptr)
        , m_select(nullptr)
    {}

    GameObjAdapter(const SmartPointer<AnimatedObjectConstruct>& obj)
        : m_obj(obj)
        , m_anim(obj.get())
        , m_select(nullptr)
    {}

    GameObjAdapter(const SmartPointer<ObjectConstruct>& obj)
        : m_obj(obj)
        , m_anim(obj.get())
        , m_select(obj.get())
    {}

    IDrawable* skin() const { return m_obj->skin(); }
    IObject* getAbstractChild(const std::string& name) const { return m_obj->getAbstractChild(name); }

    Vec2 getOffset() const { return m_obj->getOffset(); }
    void setOffset(const Vec2& v) { m_obj->setOffset(v); }

    float scale() const { return m_obj->scale(); }
    void setScale(float scale) { m_obj->setScale(scale); }

    float scaleX() const { return m_obj->scaleX(); }
    void setScaleX(float scale) { m_obj->setScaleX(scale); }

    float scaleY() const { return m_obj->scaleY(); }
    void setScaleY(float scale) { m_obj->setScaleY(scale); }

    float angle() const { return m_obj->angle(); }
    void setAngle(float angle) { m_obj->setAngle(angle); }

    bool isVisible() const { return m_obj->isVisible(); }
    void setVisible(bool value) { m_obj->setVisible(value); }

    BoundingBox box() const { return m_obj->box(); }
    BoundingBox movedBox() const { return m_obj->movedBox(); }
    BoundingBox fullBox() const { return m_obj->transformedBox(); }

    void runAnimation(const std::string& name, int channel) { if (m_anim) m_anim->runAnimation(name, channel); }

    void resetChannel(int channel) { if (m_anim) m_anim->resetChannel(channel); }
    void resetAllChannels() { if (m_anim) m_anim->resetAllChannels(); }
    bool isChannelRunning(int channel) const { return m_anim ? m_anim->isChannelRunning(channel) : false; }
    bool isChannelEmpty(int channel) const { return m_anim ? m_anim->isChannelEmpty(channel) : true; }

    void setChannelSpeed(int channel, float speed) { if (m_anim) m_anim->setChannelSpeed(channel, speed); }
    void setAllChannelsSpeed(float speed) { if (m_anim) m_anim->setAllChannelsSpeed(speed); }
    float channelSpeed(int channel) const { return m_anim ? m_anim->channelSpeed(channel) : 0.0f; }
    float defaultChannelSpeed() const { return m_anim ? m_anim->defaultChannelSpeed() : 0.0f; }

    void pauseChannel(int channel) { if (m_anim) m_anim->pauseChannel(channel); }
    void pauseAllChannels() { if (m_anim) m_anim->pauseAllChannels(); }
    void resumeChannel(int channel) { if (m_anim) m_anim->resumeChannel(channel); }
    void resumeAllChannels() { if (m_anim) m_anim->resumeAllChannels(); }
    bool isChannelPaused(int channel) const { return m_anim ? m_anim->isChannelPaused(channel) : true; }
    bool isPaused() const { return m_anim ? m_anim->isPaused() : true; }

    void setCallback(const std::function<void()>& callback) { if (m_select) m_select->setCallback(callback); }

    bool isMouseOn() const { return m_select ? m_select->isMouseOn() : false; }
    bool isPressed() const { return m_select ? m_select->isPressed() : false; }
    bool isClicked() const { return m_select ? m_select->isClicked() : false; }

    SelectionState::Enum selectionState() const { return m_select ? m_select->selectionState() : SelectionState::Disabled; }
    void setSelectionState(SelectionState::Enum state) { if (m_select) m_select->setSelectionState(state); }

    SmartPointer<IObject> getInternalObj() const { return m_obj; }

private:
    SmartPointer<InactiveObjectConstruct> m_obj;
    AnimatedObjectConstruct* m_anim;
    ObjectConstruct* m_select;
};

} }
