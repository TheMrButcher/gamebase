#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FixedOffset.h>
#include <gamebase/engine/TimeState.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/utils/Exception.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API TextEditCursor : public OffsettedPosition, public Drawable, public ISerializable {
public:
    TextEditCursor();

    void setX(float x) { static_cast<FixedOffset*>(m_offset.get())->update(Vec2(x, 0)); }
    void setYRange(float yBottom, float yTop) { m_yBottom = yBottom; m_yTop = yTop; }
    void setWidth(float width) { m_width = width; }
    void setColor(const Color& color) { m_rect.setColor(color); }
    void setPeriod(const TypedTime& period) { m_cursorPeriod = period; }

    virtual void loadResources() override
    {
        m_rect.setBox(BoundingBox(
            Vec2(0, m_yBottom), Vec2(m_width, m_yTop)));
        m_rect.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        if (needToDraw())
            m_rect.draw(position);
    }

    virtual void setBox(const BoundingBox&) override {}
    virtual BoundingBox box() const override { THROW_EX() << "TextEditCursor::box(): Not implemented"; }
    
    virtual void serialize(Serializer& s) const override;

private:
    bool needToDraw() const;

    FilledRect m_rect;
    float m_width;
    float m_yBottom;
    float m_yTop;
    TypedTime m_cursorPeriod;
};

}
