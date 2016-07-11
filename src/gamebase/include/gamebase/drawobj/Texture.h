#pragma once

#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Texture {
public:
    const std::string& imageName() const;
    void setImageName(const std::string& name);
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
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(Texture, StaticTextureRect);
};

/////////////// IMPLEMENTATION ///////////////////

GAMEBASE_DEFINE_TEXTURE_METHODS(Texture);
GAMEBASE_DEFINE_COLOR_METHODS(Texture);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Texture);

}
