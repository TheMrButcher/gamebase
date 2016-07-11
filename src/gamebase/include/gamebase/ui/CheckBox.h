#pragma once

#include <gamebase/impl/ui/CheckBox.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class CheckBox {
public:
    bool isChecked() const;
    void setChecked(bool value);
    void check();
    void uncheck();

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

    GAMEBASE_DEFINE_PIMPL(CheckBox, CheckBox);
};

/////////////// IMPLEMENTATION ///////////////////

inline bool CheckBox::isChecked() const { return m_impl->isChecked(); }
inline void CheckBox::setChecked(bool value) { m_impl->setChecked(value); }
inline void CheckBox::check() { m_impl->setChecked(true); }
inline void CheckBox::uncheck() { m_impl->setChecked(false); }
inline void CheckBox::setCallback(const std::function<void()>& callback) { m_impl->setCallback(callback); }
GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(CheckBox);

}
