#pragma once

#include <gamebase/impl/drawobj/StaticFilledRect.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class FilledRect {
public:
    void setSizes(float width, float height);

    Color color() const;
    void setColor(float r, float g, float b, float a = 1);
    void setColor(const Color& color);

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(FilledRect, StaticFilledRect);
};

/////////////// IMPLEMENTATION ///////////////////

inline void FilledRect::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }
GAMEBASE_DEFINE_COLOR_METHODS(FilledRect);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(FilledRect);

}
