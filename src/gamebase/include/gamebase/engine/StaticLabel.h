#pragma once

#include <gamebase/engine/LabelBase.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {

class GAMEBASE_API StaticLabel : public LabelBase, public OffsettedPosition,
    public Registrable, public ISerializable {
public:
    StaticLabel(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : LabelBase(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}

    virtual Transform2 position() const override
    {
        auto transform = OffsettedPosition::position();
        transform.offset.x = fround(transform.offset.x);
        transform.offset.y = fround(transform.offset.y);
        return transform;
    }

    virtual void setText(const std::string& text) override
    {
        m_text = text;
        if (m_rect.isValid() && m_box->isValid()) {
            LabelBase::setBox(m_box->get());
            loadResources();
        }
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        LabelBase::setBox(m_box->get());
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

typedef StaticLabel Label;

}
