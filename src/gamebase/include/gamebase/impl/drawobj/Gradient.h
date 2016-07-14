/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/engine/Direction.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/graphics/GLBuffers.h>
#include <gamebase/graphics/Color.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Gradient : public Drawable, public OffsettedPosition, public ISerializable {
public:
    Gradient(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr)
        : Drawable(this)
        , OffsettedPosition(position)
        , m_box(box)
    {}
    
    void setFixedBox(float width, float height);

    Direction::Enum direction() const { return m_dir; }
    void setDirection(Direction::Enum value) { m_dir = value; reload(); }

    const Color& color1() const { return m_color1; }
    void setColor1(const Color& color1) { m_color1 = color1; reload(); }

    const Color& color2() const { return m_color2; }
    void setColor2(const Color& color2) { m_color2 = color2; reload(); }

    virtual void loadResources() override { reload(); }
    virtual void drawAt(const Transform2& position) const override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override { return m_box->get(); }
    
    virtual void serialize(Serializer& s) const override;

private:
    void reload();

    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_dir;
    Color m_color1;
    Color m_color2;

    GLBuffers m_buffers;
};

} }
