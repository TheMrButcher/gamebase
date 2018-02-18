/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "Frame.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {

Frame::Frame()
    : m_frameIndex(0)
    , m_lineIndex(0)
    , m_time(0)
{}

Frame::Frame(int frameIndex, int lineIndex, Time time)
    : m_frameIndex(frameIndex)
    , m_lineIndex(lineIndex)
    , m_time(time)
{}

void Frame::serialize(Serializer& s) const
{
    s << "lineIndex" << m_lineIndex
        << "frameIndex" << m_frameIndex
        << "time" << m_time;
}

std::unique_ptr<IObject> deserializeFrame(Deserializer& deserializer)
{
    DESERIALIZE(int, lineIndex);
    DESERIALIZE(int, frameIndex);
    DESERIALIZE(Time, time);
    return std::make_unique<Frame>(frameIndex, lineIndex, time);
}

REGISTER_CLASS(Frame);

} }
