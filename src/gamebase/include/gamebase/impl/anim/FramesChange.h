/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class Frame;

class GAMEBASE_API FramesChange : public IAnimation, public ISerializable {
public:
    FramesChange(
        const std::string& atlasName,
        int startFrameIndex,
        int lastFrameIndex,
        Time period,
        int repeatTimes);

    FramesChange(
        const std::string& atlasName,
        std::vector<std::shared_ptr<Frame>>&& frames);

    virtual void load(const PropertiesRegister& props) override;
    virtual void start() override;
    virtual Time step(Time t) override;

    virtual bool isFinished() const override
    {
        return m_curFrameIndex >= m_frames.size();
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::string m_atlasName;
    std::shared_ptr<Value<int>> m_frameProperty;
    std::shared_ptr<Value<int>> m_lineProperty;
    std::vector<std::shared_ptr<Frame>> m_frames;
    Time m_curTime;
    size_t m_curFrameIndex;
    bool m_needUpdateFrame;
};

} }
