#pragma once

#include <gamebase/impl/ui/TextBox.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class TextBox {
public:
    const std::string& text() const;
    void setText(const std::string& text);

    void setCallback(const std::function<void()>& callback);

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

    bool isEnabled() const;
    void setEnabled(bool value);
    void enable();
    void disable();

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(TextBox, TextBox);
};

/////////////// IMPLEMENTATION ///////////////////

GAMEBASE_DEFINE_TEXT_METHODS(TextBox);
inline void TextBox::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(TextBox);

}
