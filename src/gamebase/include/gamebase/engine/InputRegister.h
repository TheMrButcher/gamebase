#pragma once

#include <gamebase/engine/SpecificInputRegister.h>
#include <gamebase/engine/ProjectionTransform.h>
#include <gamebase/engine/SpecialKey.h>
#include <gamebase/engine/MouseButton.h>
#include <gamebase/math/Vector2.h>

namespace gamebase {

class InputRegister {
public:
    InputRegister() : m_changedPosition(false) {}

    bool isPressed(char key) const
    {
        return keys.isPressed(key);
    }

    bool isJustPressed(char key) const
    {
        return keys.isJustPressed(key);
    }

    bool isJustOutpressed(char key) const
    {
        return keys.isJustOutpressed(key);
    }

    bool isPressed(SpecialKey::Enum key) const
    {
        return specialKeys.isPressed(key);
    }

    bool isJustPressed(SpecialKey::Enum key) const
    {
        return specialKeys.isJustPressed(key);
    }

    bool isJustOutpressed(SpecialKey::Enum key) const
    {
        return specialKeys.isJustOutpressed(key);
    }

    bool isPressed(MouseButton::Enum key) const
    {
        return mouseButtons.isPressed(key);
    }

    bool isJustPressed(MouseButton::Enum key) const
    {
        return mouseButtons.isJustPressed(key);
    }

    bool isJustOutpressed(MouseButton::Enum key) const
    {
        return mouseButtons.isJustOutpressed(key);
    }

    void setMousePosition(int x, int y)
    {
        setMousePosition(convertMouseCoords(x, y));
    }

    void setMousePosition(const Vec2& normalizedPosition)
    {
        m_mousePosition = normalizedPosition;
        m_changedPosition = true;
    }

    const Vec2& mousePosition() const
    {
        return m_mousePosition;
    }

    bool changedPosition() const
    {
        return m_changedPosition;
    }

    void step()
    {
        keys.step();
        specialKeys.step();
        mouseButtons.step();
        m_changedPosition = false;
    }

    void reset()
    {
        keys.reset();
        specialKeys.reset();
        mouseButtons.reset();
        m_changedPosition = false;
    }

public:
    SpecificInputRegister<unsigned char> keys;
    SpecificInputRegister<SpecialKey::Enum> specialKeys;
    SpecificInputRegister<MouseButton::Enum> mouseButtons;

private:
    Vec2 m_mousePosition;
    bool m_changedPosition;
};

}
