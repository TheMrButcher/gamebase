#pragma once

#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/engine/Direction.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/graphics/GLBuffers.h>

namespace gamebase {

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
    void setDirection(Direction::Enum value) { m_dir = value; }

    const Color& color1() const { return m_color1; }
    void setColor1(const Color& color1) { m_color1 = color1; }

    const Color& color2() const { return m_color2; }
    void setColor2(const Color& color2) { m_color2 = color2; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override { return m_box->get(); }
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    Direction::Enum m_dir;
    Color m_color1;
    Color m_color2;

    GLBuffers m_buffers;
};

}
