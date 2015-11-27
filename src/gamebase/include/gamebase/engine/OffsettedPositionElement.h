#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/PositionElement.h>

namespace gamebase {

class GAMEBASE_API OffsettedPositionElement : public PositionElement {
public:
    OffsettedPositionElement(const Vec2& offset = Vec2())
    {
        m_pos.offset = offset;
    }

    virtual Vec2 getOffset() const override { return m_pos.offset; }
    virtual void setOffset(const Vec2& v) override { m_pos.offset = v; }

    virtual Transform2 position() const override
    {
        return m_pos;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& serializer) const override;

private:
    Transform2 m_pos;
};

}
