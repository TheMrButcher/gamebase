/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "SpecialKeyConverter.h"

namespace gamebase { namespace impl {

SpecialKey::Enum convertGLUTCode(int key)
{
    switch (key) {
    case GLUT_KEY_F1: return SpecialKey::F1;
    case GLUT_KEY_F2: return SpecialKey::F2;
    case GLUT_KEY_F3: return SpecialKey::F3;
    case GLUT_KEY_F4: return SpecialKey::F4;
    case GLUT_KEY_F5: return SpecialKey::F5;
    case GLUT_KEY_F6: return SpecialKey::F6;
    case GLUT_KEY_F7: return SpecialKey::F7;
    case GLUT_KEY_F8: return SpecialKey::F8;
    case GLUT_KEY_F9: return SpecialKey::F9;
    case GLUT_KEY_F10: return SpecialKey::F10;
    case GLUT_KEY_F11: return SpecialKey::F11;
    case GLUT_KEY_F12: return SpecialKey::F12;
    case GLUT_KEY_LEFT: return SpecialKey::Left;
    case GLUT_KEY_UP: return SpecialKey::Up;
    case GLUT_KEY_RIGHT: return SpecialKey::Right;
    case GLUT_KEY_DOWN: return SpecialKey::Down;
    case GLUT_KEY_PAGE_UP: return SpecialKey::PageUp;
    case GLUT_KEY_PAGE_DOWN: return SpecialKey::PageDown;
    case GLUT_KEY_HOME: return SpecialKey::Home;
    case GLUT_KEY_END: return SpecialKey::End;
    case GLUT_KEY_INSERT: return SpecialKey::Insert;
    case GLUT_KEY_SHIFT_L: return SpecialKey::ShiftLeft;
    case GLUT_KEY_SHIFT_R: return SpecialKey::ShiftRight;
    case GLUT_KEY_CTRL_L: return SpecialKey::CtrlLeft;
    case GLUT_KEY_CTRL_R: return SpecialKey::CtrlRight;
    case GLUT_KEY_ALT_L: return SpecialKey::AltLeft;
    case GLUT_KEY_ALT_R: return SpecialKey::AltRight;
    default: return SpecialKey::Unknown;
    }
}

} }
