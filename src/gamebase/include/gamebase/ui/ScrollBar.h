#pragma once

#include <gamebase/impl/ui/ScrollBar.h>
#include <gamebase/impl/pubhelp/Helpers.h>

namespace gamebase {

class ScrollBar {
public:
    void connect(float& value);
    void setRange(float minVal, float maxVal);
    void setBarSize(float size);
    void update();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;

    GAMEBASE_DEFINE_PIMPL(ScrollBar, ScrollBar);
};

/////////////// IMPLEMENTATION ///////////////////

inline void ScrollBar::connect(float& value) { m_impl->setControlledValue(std::make_shared<impl::FloatValue>(&value)); }
inline void ScrollBar::setRange(float minVal, float maxVal) { m_impl->setRange(minVal, maxVal); }
inline void ScrollBar::setBarSize(float size) { m_impl->setVisibleZoneSize(size); }
inline void ScrollBar::update() { m_impl->move(0); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ScrollBar);

}
