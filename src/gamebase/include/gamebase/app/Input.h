/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/AppHelpers.h>

namespace gamebase {

class Input {
public:
    Vec2 mousePos() const;
    float wheel() const;

    bool pressed(InputKey::Enum key) const;
    bool justPressed(InputKey::Enum key) const;
    bool justOutpressed(InputKey::Enum key) const;

    bool pressed(char key) const;
    bool justPressed(char key) const;
    bool justOutpressed(char key) const;

    GAMEBASE_DEFINE_APP_INPUT_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline Vec2 Input::mousePos() const { return m_impl->mousePosition(); }
inline float Input::wheel() const { return m_impl->wheel; }
inline bool Input::pressed(InputKey::Enum key) const { return m_impl->isPressed(key); }
inline bool Input::justPressed(InputKey::Enum key) const { return m_impl->isJustPressed(key); }
inline bool Input::justOutpressed(InputKey::Enum key) const { return m_impl->isJustOutpressed(key); }
inline bool Input::pressed(char key) const { return m_impl->isPressed(key); }
inline bool Input::justPressed(char key) const { return m_impl->isJustPressed(key); }
inline bool Input::justOutpressed(char key) const { return m_impl->isJustOutpressed(key); }
}
