/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/LabelBase.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relbox/IResizable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API StaticLabel : public LabelBase, public OffsettedPosition,
    public Registrable, public ISerializable, public IResizable {
public:
    StaticLabel(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : LabelBase(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}

    virtual void setFixedBox(float width, float height) override;

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
        m_parentBox = allowedBox;
        m_box->setParentBox(allowedBox);
        LabelBase::setBox(m_box->get());
        setPositionBoxes(allowedBox, box());
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    }
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    BoundingBox m_parentBox;
};

typedef StaticLabel Label;

} }
