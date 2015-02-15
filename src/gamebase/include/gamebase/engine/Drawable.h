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

private:
    const IPositionable* m_drawPosition;
    bool m_visible;
};

}
