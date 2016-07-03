#pragma once

#include <gamebase/impl/pubhelp/DrawObjHelpers.h>

namespace gamebase {

class DrawObj {
public:
    template <typename T> T cast() const;

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    GAMEBASE_DEFINE_DRAW_OBJ_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

template <typename T> inline T DrawObj::cast() const { return impl::wrap<T>(impl::unwrapSmart(*this)); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(DrawObj);

}
