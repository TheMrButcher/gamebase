/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "KeyConverter.h"
#include <SFML/Window/Keyboard.hpp>

namespace gamebase { namespace impl {

InputKey::Enum convertCode(int key)
{
    switch (key) {
	case sf::Keyboard::Space: return InputKey::Space;
    case sf::Keyboard::Num0: return InputKey::Num0;
    case sf::Keyboard::Num1: return InputKey::Num1;
    case sf::Keyboard::Num2: return InputKey::Num2;
    case sf::Keyboard::Num3: return InputKey::Num3;
    case sf::Keyboard::Num4: return InputKey::Num4;
    case sf::Keyboard::Num5: return InputKey::Num5;
    case sf::Keyboard::Num6: return InputKey::Num6;
    case sf::Keyboard::Num7: return InputKey::Num7;
    case sf::Keyboard::Num8: return InputKey::Num8;
    case sf::Keyboard::Num9: return InputKey::Num9;
    case sf::Keyboard::A: return InputKey::A;
    case sf::Keyboard::B: return InputKey::B;
    case sf::Keyboard::C: return InputKey::C;
    case sf::Keyboard::D: return InputKey::D;
    case sf::Keyboard::E: return InputKey::E;
    case sf::Keyboard::F: return InputKey::F;
    case sf::Keyboard::G: return InputKey::G;
    case sf::Keyboard::H: return InputKey::H;
    case sf::Keyboard::I: return InputKey::I;
    case sf::Keyboard::J: return InputKey::J;
    case sf::Keyboard::K: return InputKey::K;
    case sf::Keyboard::L: return InputKey::L;
    case sf::Keyboard::M: return InputKey::M;
    case sf::Keyboard::N: return InputKey::N;
    case sf::Keyboard::O: return InputKey::O;
    case sf::Keyboard::P: return InputKey::P;
    case sf::Keyboard::Q: return InputKey::Q;
    case sf::Keyboard::R: return InputKey::R;
    case sf::Keyboard::S: return InputKey::S;
    case sf::Keyboard::T: return InputKey::T;
    case sf::Keyboard::U: return InputKey::U;
    case sf::Keyboard::V: return InputKey::V;
    case sf::Keyboard::W: return InputKey::W;
    case sf::Keyboard::X: return InputKey::X;
    case sf::Keyboard::Y: return InputKey::Y;
    case sf::Keyboard::Z: return InputKey::Z;
    case sf::Keyboard::Escape: return InputKey::Escape;
    case sf::Keyboard::Return: return InputKey::Enter;
    case sf::Keyboard::BackSpace: return InputKey::BackSpace;
    case sf::Keyboard::Tab: return InputKey::Tab;
    case sf::Keyboard::F1: return InputKey::F1;
    case sf::Keyboard::F2: return InputKey::F2;
    case sf::Keyboard::F3: return InputKey::F3;
    case sf::Keyboard::F4: return InputKey::F4;
    case sf::Keyboard::F5: return InputKey::F5;
    case sf::Keyboard::F6: return InputKey::F6;
    case sf::Keyboard::F7: return InputKey::F7;
    case sf::Keyboard::F8: return InputKey::F8;
    case sf::Keyboard::F9: return InputKey::F9;
    case sf::Keyboard::F10: return InputKey::F10;
    case sf::Keyboard::F11: return InputKey::F11;
    case sf::Keyboard::F12: return InputKey::F12;
    case sf::Keyboard::Left: return InputKey::Left;
    case sf::Keyboard::Up: return InputKey::Up;
    case sf::Keyboard::Right: return InputKey::Right;
    case sf::Keyboard::Down: return InputKey::Down;
    case sf::Keyboard::PageUp: return InputKey::PageUp;
    case sf::Keyboard::PageDown: return InputKey::PageDown;
    case sf::Keyboard::Home: return InputKey::Home;
    case sf::Keyboard::End: return InputKey::End;
    case sf::Keyboard::Insert: return InputKey::Insert;
    case sf::Keyboard::Delete: return InputKey::Delete;
    case sf::Keyboard::LShift: return InputKey::ShiftLeft;
    case sf::Keyboard::RShift: return InputKey::ShiftRight;
    case sf::Keyboard::LControl: return InputKey::CtrlLeft;
    case sf::Keyboard::RControl: return InputKey::CtrlRight;
    case sf::Keyboard::LAlt: return InputKey::AltLeft;
    case sf::Keyboard::RAlt: return InputKey::AltRight;
    case sf::Keyboard::Numpad0: return InputKey::Numpad0;
    case sf::Keyboard::Numpad1: return InputKey::Numpad1;
    case sf::Keyboard::Numpad2: return InputKey::Numpad2;
    case sf::Keyboard::Numpad3: return InputKey::Numpad3;
    case sf::Keyboard::Numpad4: return InputKey::Numpad4;
    case sf::Keyboard::Numpad5: return InputKey::Numpad5;
    case sf::Keyboard::Numpad6: return InputKey::Numpad6;
    case sf::Keyboard::Numpad7: return InputKey::Numpad7;
    case sf::Keyboard::Numpad8: return InputKey::Numpad8;
    case sf::Keyboard::Numpad9: return InputKey::Numpad9;
    case sf::Keyboard::LBracket: return InputKey::BracketLeft;
    case sf::Keyboard::RBracket: return InputKey::BracketRight;
    case sf::Keyboard::SemiColon: return InputKey::SemiColon;
    case sf::Keyboard::Quote: return InputKey::Quote;
    case sf::Keyboard::Comma: return InputKey::Comma;
    case sf::Keyboard::Period: return InputKey::Dot;
    case sf::Keyboard::Slash: return InputKey::Slash;
    case sf::Keyboard::BackSlash: return InputKey::BackSlash;
    case sf::Keyboard::Tilde: return InputKey::Tilde;
    case sf::Keyboard::Equal: return InputKey::Equal;
    case sf::Keyboard::Dash: return InputKey::Dash;
    case sf::Keyboard::Add: return InputKey::Add;
    case sf::Keyboard::Subtract: return InputKey::Subtract;
    case sf::Keyboard::Multiply: return InputKey::Multiply;
    case sf::Keyboard::Divide: return InputKey::Divide;
    case sf::Keyboard::Pause: return InputKey::Pause;
    default: return InputKey::Unknown;
    }
}

} }
