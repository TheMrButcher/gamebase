#pragma once

#include <gamebase/impl/adapt/IDrawObjAdapter.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class DrawObj {
public:
    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    GAMEBASE_DEFINE_PIMPL_STD_SP(DrawObj, IDrawObjAdapter);
};

/////////////// IMPLEMENTATION ///////////////////

GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(DrawObj);

}
