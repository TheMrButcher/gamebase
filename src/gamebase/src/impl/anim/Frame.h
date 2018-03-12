/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/common/Time.h>

namespace gamebase { namespace impl {

class Frame : public ISerializable {
public:
    Frame();
    Frame(int frameIndex, int lineIndex, Time time);

    int frameIndex() const { return m_frameIndex; }
    int lineIndex() const { return m_lineIndex; }
    Time time() const { return m_time; }

    virtual void serialize(Serializer& s) const override;

private:
    int m_frameIndex;
    int m_lineIndex;
    Time m_time;
};

} }
