#pragma once

#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/utils/Exception.h>
#include <vector>

namespace gamebase {

class TextEditCursor : public OffsettedPosition, public IDrawable {
public:
    TextEditCursor()
        : OffsettedPosition(std::make_shared<FixedOffset>())
    {
        m_width = 1.0f;
        m_rect.setColor(Color(0, 0, 0));
    }

    void setX(float x) { static_cast<FixedOffset*>(m_offset.get())->update(Vec2(x, 0)); }
    void setYRange(float yBottom, float yTop) { m_yBottom = yBottom; m_yTop = yTop; }
    void setWidth(float width) { m_width = width; }
    void setColor(const Color& color) { m_rect.setColor(color); }

    virtual void loadResources() override
    {
        m_rect.setBox(BoundingBox(
            Vec2(0, m_yBottom), Vec2(m_width, m_yTop)));
        m_rect.loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_rect.draw(transform() * globalPosition);
    }

    virtual void setBox(const BoundingBox&) override {}
    virtual BoundingBox box() const override { THROW_EX() << "TextEditCursor::box(): Not implemented"; }

private:
    FilledRect m_rect;
    float m_width;
    float m_yBottom;
    float m_yTop;
};

}
