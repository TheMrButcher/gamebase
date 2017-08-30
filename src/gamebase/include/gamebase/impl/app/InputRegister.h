/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/app/InputKey.h>
#include <gamebase/impl/app/SpecificInputRegister.h>
#include <gamebase/impl/app/MouseButton.h>
#include <gamebase/impl/tools/ProjectionTransform.h>
#include <gamebase/math/Vector2.h>

namespace gamebase { namespace impl {

class InputRegister {
public:
    InputRegister() : m_changedPosition(false), wheel(0) {}

    bool isPressed(InputKey::Enum key) const
    {
        return keys.isPressed(key);
    }

    bool isJustPressed(InputKey::Enum key) const
    {
        return keys.isJustPressed(key);
    }

    bool isJustOutpressed(InputKey::Enum key) const
    {
        return keys.isJustOutpressed(key);
    }

    bool isPressed(char ch) const
    {
        auto key = convertChar(ch);
        if (key == InputKey::Unknown)
            return false;
        return keys.isPressed(key);
    }

    bool isJustPressed(char ch) const
    {
        auto key = convertChar(ch);
        if (key == InputKey::Unknown)
            return false;
        return keys.isJustPressed(key);
    }

    bool isJustOutpressed(char ch) const
    {
        auto key = convertChar(ch);
        if (key == InputKey::Unknown)
            return false;
        return keys.isJustOutpressed(key);
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
        chars.clear();
        wheel = 0;
        m_changedPosition = false;
    }

public:
    SpecificInputRegister<InputKey::Enum> keys;
    std::vector<uint32_t> chars;
    float wheel;

private:
    InputKey::Enum convertChar(char key) const
    {
        if (key >= 'A' && key <= 'Z')
            key += 'a' - 'A';
        if (key == ' '
            || (key >= '0' && key <= '9')
            || (key >= 'a' && key <= 'z'))
            return static_cast<InputKey::Enum>(key);
        switch (key) {
        case 10: return InputKey::Enter;
        case 13: return InputKey::Enter;
        case 27: return InputKey::Escape;
        case 8: return InputKey::BackSpace;
        case 127: return InputKey::Delete;
        case '-': return InputKey::Dash;
        case '=': return InputKey::Equal;
        case '\t': return InputKey::Tab;
        }
        return InputKey::Unknown;
    }

    Vec2 m_mousePosition;
    bool m_changedPosition;
};

} }
