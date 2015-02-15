#pragma once

#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/IRelativeOffset.h>

namespace gamebase {

class OffsettedPosition : public IPositionable {
public:
    OffsettedPosition() {}

    OffsettedPosition(const std::shared_ptr<IRelativeOffset>& offset)
        : m_offset(offset)
    {}

    void setPosition(const std::shared_ptr<IRelativeOffset>& offset)
    {
        m_offset = offset;
    }

    virtual Transform2 position() const override
    {
        return m_offset ? m_offset->get() : Transform2();
    }

protected:
    std::shared_ptr<IRelativeOffset> m_offset;
};

}
