#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/engine/IAction.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API ActionInAnimation : public IAnimation, public ISerializable {
public:
    static const int Infinity = -1;

    ActionInAnimation(const std::shared_ptr<IAction>& action)
        : m_action(action)
    {}

    virtual void load(const PropertiesRegister& props) override
    {
        m_action->load(props);
    }

    virtual void start() override
    {
        m_done = false;
    }

    virtual void step() override
    {
        m_action->exec();
        m_done = true;
    }

    virtual bool isFinished() const override
    {
        return m_done;
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::shared_ptr<IAction> m_action;
    bool m_done;
};

}
