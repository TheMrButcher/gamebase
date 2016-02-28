#pragma once

#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API StaticFilledRect : public FilledRect, public OffsettedPosition,
    public Registrable, public ISerializable {
public:
    StaticFilledRect(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : FilledRect(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        FilledRect::setBox(m_box->get());
        setPositionBoxes(allowedBox, box());
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("colorA", &m_color.a);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    }
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
};

typedef StaticFilledRect Filled;

}
