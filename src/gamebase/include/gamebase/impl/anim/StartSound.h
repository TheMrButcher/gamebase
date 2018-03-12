/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/anim/IAnimation.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API StartSound : public IAnimation, public ISerializable {
public:
    StartSound(const std::string& filePath, int channel)
        : m_filePath(filePath)
        , m_channel(channel)
    {}

    virtual void load(const PropertiesRegister&) override {}
    virtual void start() override { m_done = false; }
    virtual Time step(Time t) override;
    virtual bool isFinished() const override { return m_done; }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::string m_filePath;
    int m_channel;
    bool m_done;
};

} }
