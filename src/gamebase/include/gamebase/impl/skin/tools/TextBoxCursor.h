/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <gamebase/impl/app/TimeState.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/graphics/Color.h>
#include <gamebase/tools/Exception.h>
#include <vector>

namespace gamebase { namespace impl {

class GAMEBASE_API TextBoxCursor : public OffsettedPosition, public Drawable, public ISerializable {
public:
    TextBoxCursor();

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
    virtual BoundingBox box() const override { THROW_EX() << "TextBoxCursor::box(): Not implemented"; }
    
    virtual void serialize(Serializer& s) const override;

private:
    bool needToDraw() const;

    FilledRect m_rect;
    float m_width;
    float m_yBottom;
    float m_yTop;
    TypedTime m_cursorPeriod;
};

} }
