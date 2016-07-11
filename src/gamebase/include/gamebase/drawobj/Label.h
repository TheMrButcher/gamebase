#pragma once

#include <gamebase/impl/drawobj/StaticLabel.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Label {
public:
    const std::string& text() const;
    void setText(const std::string& text);

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

    GAMEBASE_DEFINE_PIMPL(Label, StaticLabel);
};

/////////////// IMPLEMENTATION ///////////////////

GAMEBASE_DEFINE_TEXT_METHODS(Label);
GAMEBASE_DEFINE_COLOR_METHODS(Label);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Label);

}
