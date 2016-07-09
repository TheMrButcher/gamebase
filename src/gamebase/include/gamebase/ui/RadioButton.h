#pragma once

#include <gamebase/impl/ui/RadioButton.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class RadioButton {
public:
    int id() const;

    bool isChecked() const;
    void check();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    bool isEnabled() const;
    void setEnabled(bool value);
    void enable();
    void disable();

    GAMEBASE_DEFINE_PIMPL(RadioButton, RadioButton);
};

/////////////// IMPLEMENTATION ///////////////////

inline int RadioButton::id() const { return m_impl->indexInGroup(); }
inline bool RadioButton::isChecked() const { return m_impl->isChecked(); }
inline void RadioButton::check() { m_impl->setChecked(); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(RadioButton);

}
