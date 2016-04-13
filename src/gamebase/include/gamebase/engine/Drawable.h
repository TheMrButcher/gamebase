#pragma once

#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IPositionable.h>

namespace gamebase {

class Drawable : public virtual IDrawable {
public:
    Drawable(const IPositionable* position = nullptr)
        : m_drawPosition(position)
        , m_visible(true)
    {}
    
    virtual void draw(const Transform2& globalPosition) const override
    {
        if (!isVisible())
            return;
        drawAt(m_drawPosition
            ? m_drawPosition->position() * globalPosition
            : globalPosition);
    }

    virtual void setVisible(bool visible) override
    {
        m_visible = visible;
    }

    virtual bool isVisible() const override
    {
        return m_visible;
    }

    BoundingBox movedBox() const
    {
        if (!m_drawPosition)
            return box();
        BoundingBox myBox = box();
        myBox.move(m_drawPosition->position().offset);
        return myBox;
    }

    BoundingBox transformedBox() const
    {
        if (!m_drawPosition)
            return box();
        BoundingBox myBox = box();
        myBox.transform(m_drawPosition->position());
        return myBox;
    }

    const IPositionable* drawPosition() const { return m_drawPosition; }

private:
    const IPositionable* m_drawPosition;
    bool m_visible;
};

GAMEBASE_API bool isMouseOn(Drawable* drawable);

}
