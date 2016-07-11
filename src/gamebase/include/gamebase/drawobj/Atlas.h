#pragma once

#include <gamebase/impl/drawobj/Atlas.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class Atlas {
public:
    int frame() const;
    void setFrame(int value);

    int line() const;
    void setLine(int value);

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

    GAMEBASE_DEFINE_PIMPL(Atlas, Atlas);
};

/////////////// IMPLEMENTATION ///////////////////

inline int Atlas::frame() const { return m_impl->frameIndex(); }
inline void Atlas::setFrame(int value) { m_impl->setFrameIndex(value); }
inline int Atlas::line() const { return m_impl->lineIndex(); }
inline void Atlas::setLine(int value) { m_impl->setLineIndex(value); }
GAMEBASE_DEFINE_TEXTURE_METHODS(Atlas);
GAMEBASE_DEFINE_COLOR_METHODS(Atlas);
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(Atlas);

}
