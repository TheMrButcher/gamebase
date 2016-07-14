/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/AppHelpers.h>

namespace gamebase {

class Input {
public:
    Vec2 mousePos() const;
    int wheel() const;

    ////////////// SIMPLE CHARACTERS ////////////////////////////////
    bool pressed(char key) const;
    bool justPressed(char key) const;
    bool justOutpressed(char key) const;


    ////////////// MOUSE BUTTONS ////////////////////////////////////
    bool leftButtonPressed() const;
    bool leftButtonJustPressed() const;
    bool leftButtonJustOutpressed() const;

    bool midButtonPressed() const;
    bool midButtonJustPressed() const;
    bool midButtonJustOutpressed() const;

    bool rightButtonPressed() const;
    bool rightButtonJustPressed() const;
    bool rightButtonJustOutpressed() const;


    ////////////// ENTER ////////////////////////////////////////////
    bool enterPressed() const;
    bool enterJustPressed() const;
    bool enterJustOutpressed() const;


    ////////////// ESCAPE ///////////////////////////////////////////
    bool escapePressed() const;
    bool escapeJustPressed() const;
    bool escapeJustOutpressed() const;


    ////////////// FUNCTIONAL BUTTONS ///////////////////////////////
    bool funcPressed(int index) const;
    bool funcJustPressed(int index) const;
    bool funcJustOutpressed(int index) const;

    
    ////////////// ARROWS ///////////////////////////////////////////
    bool leftPressed() const;
    bool leftJustPressed() const;
    bool leftJustOutpressed() const;
    
    bool upPressed() const;
    bool upJustPressed() const;
    bool upJustOutpressed() const;
    
    bool rightPressed() const;
    bool rightJustPressed() const;
    bool rightJustOutpressed() const;
    
    bool downPressed() const;
    bool downJustPressed() const;
    bool downJustOutpressed() const;


    ////////////// PAGE UP AND PAGE DOWN ////////////////////////////
    bool pageUpPressed() const;
    bool pageUpJustPressed() const;
    bool pageUpJustOutpressed() const;

    bool pageDownPressed() const;
    bool pageDownJustPressed() const;
    bool pageDownJustOutpressed() const;

    
    ////////////// HOME, END AND INSERT /////////////////////////////
    bool homePressed() const;
    bool homeJustPressed() const;
    bool homeJustOutpressed() const;

    bool endPressed() const;
    bool endJustPressed() const;
    bool endJustOutpressed() const;

    bool insertPressed() const;
    bool insertJustPressed() const;
    bool insertJustOutpressed() const;

    
    ////////////// SHIFT (LEFT AND RIGHT) ///////////////////////////
    bool shiftLeftPressed() const;
    bool shiftLeftJustPressed() const;
    bool shiftLeftJustOutpressed() const;
    
    bool shiftRightPressed() const;
    bool shiftRightJustPressed() const;
    bool shiftRightJustOutpressed() const;
    

    ////////////// CTRL (LEFT AND RIGHT) ////////////////////////////
    bool ctrlLeftPressed() const;
    bool ctrlLeftJustPressed() const;
    bool ctrlLeftJustOutpressed() const;
    
    bool ctrlRightPressed() const;
    bool ctrlRightJustPressed() const;
    bool ctrlRightJustOutpressed() const;


    ////////////// ALT (LEFT AND RIGHT) /////////////////////////////
    bool altLeftPressed() const;
    bool altLeftJustPressed() const;
    bool altLeftJustOutpressed() const;
    
    bool altRightPressed() const;
    bool altRightJustPressed() const;
    bool altRightJustOutpressed() const;

    GAMEBASE_DEFINE_APP_INPUT_PIMPL();
};

/////////////// IMPLEMENTATION ///////////////////

inline Vec2 Input::mousePos() const { return m_impl->mousePosition(); }
inline int Input::wheel() const { return m_impl->wheel; }
inline bool Input::pressed(char key) const { return m_impl->isPressed(key); }
inline bool Input::justPressed(char key) const { return m_impl->isJustPressed(key); }
inline bool Input::justOutpressed(char key) const { return m_impl->isJustOutpressed(key); }
inline bool Input::leftButtonPressed() const { return m_impl->isPressed(impl::MouseButton::Left); }
inline bool Input::leftButtonJustPressed() const { return m_impl->isJustPressed(impl::MouseButton::Left); }
inline bool Input::leftButtonJustOutpressed() const { return m_impl->isJustOutpressed(impl::MouseButton::Left); }
inline bool Input::midButtonPressed() const { return m_impl->isPressed(impl::MouseButton::Middle); }
inline bool Input::midButtonJustPressed() const { return m_impl->isJustPressed(impl::MouseButton::Middle); }
inline bool Input::midButtonJustOutpressed() const { return m_impl->isJustOutpressed(impl::MouseButton::Middle); }
inline bool Input::rightButtonPressed() const { return m_impl->isPressed(impl::MouseButton::Right); }
inline bool Input::rightButtonJustPressed() const { return m_impl->isJustPressed(impl::MouseButton::Right); }
inline bool Input::rightButtonJustOutpressed() const { return m_impl->isJustOutpressed(impl::MouseButton::Right); }
inline bool Input::enterPressed() const { return m_impl->isPressed(char(10)) || m_impl->isPressed(char(13)); }
inline bool Input::enterJustPressed() const { return m_impl->isJustPressed(char(10)) || m_impl->isPressed(char(13)); }
inline bool Input::enterJustOutpressed() const { return m_impl->isJustOutpressed(char(10)) || m_impl->isPressed(char(13)); }
inline bool Input::escapePressed() const { return m_impl->isPressed(char(27)); }
inline bool Input::escapeJustPressed() const { return m_impl->isJustPressed(char(27)); }
inline bool Input::escapeJustOutpressed() const { return m_impl->isJustOutpressed(char(27)); }
inline bool Input::funcPressed(int index) const { return m_impl->isPressed(static_cast<impl::SpecialKey::Enum>(static_cast<int>(impl::SpecialKey::F1) + index - 1)); }
inline bool Input::funcJustPressed(int index) const { return m_impl->isJustPressed(static_cast<impl::SpecialKey::Enum>(static_cast<int>(impl::SpecialKey::F1) + index - 1)); }
inline bool Input::funcJustOutpressed(int index) const { return m_impl->isJustOutpressed(static_cast<impl::SpecialKey::Enum>(static_cast<int>(impl::SpecialKey::F1) + index - 1)); }
inline bool Input::leftPressed() const { return m_impl->isPressed(impl::SpecialKey::Left); }
inline bool Input::leftJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Left); }
inline bool Input::leftJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Left); }
inline bool Input::upPressed() const { return m_impl->isPressed(impl::SpecialKey::Up); }
inline bool Input::upJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Up); }
inline bool Input::upJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Up); }
inline bool Input::rightPressed() const { return m_impl->isPressed(impl::SpecialKey::Right); }
inline bool Input::rightJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Right); }
inline bool Input::rightJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Right); }
inline bool Input::downPressed() const { return m_impl->isPressed(impl::SpecialKey::Down); }
inline bool Input::downJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Down); }
inline bool Input::downJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Down); }
inline bool Input::pageUpPressed() const { return m_impl->isPressed(impl::SpecialKey::PageUp); }
inline bool Input::pageUpJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::PageUp); }
inline bool Input::pageUpJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::PageUp); }
inline bool Input::pageDownPressed() const { return m_impl->isPressed(impl::SpecialKey::PageDown); }
inline bool Input::pageDownJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::PageDown); }
inline bool Input::pageDownJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::PageDown); }
inline bool Input::homePressed() const { return m_impl->isPressed(impl::SpecialKey::Home); }
inline bool Input::homeJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Home); }
inline bool Input::homeJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Home); }
inline bool Input::endPressed() const { return m_impl->isPressed(impl::SpecialKey::End); }
inline bool Input::endJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::End); }
inline bool Input::endJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::End); }
inline bool Input::insertPressed() const { return m_impl->isPressed(impl::SpecialKey::Insert); }
inline bool Input::insertJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::Insert); }
inline bool Input::insertJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::Insert); }
inline bool Input::shiftLeftPressed() const { return m_impl->isPressed(impl::SpecialKey::ShiftLeft); }
inline bool Input::shiftLeftJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::ShiftLeft); }
inline bool Input::shiftLeftJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::ShiftLeft); }
inline bool Input::shiftRightPressed() const { return m_impl->isPressed(impl::SpecialKey::ShiftRight); }
inline bool Input::shiftRightJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::ShiftRight); }
inline bool Input::shiftRightJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::ShiftRight); }
inline bool Input::ctrlLeftPressed() const { return m_impl->isPressed(impl::SpecialKey::CtrlLeft); }
inline bool Input::ctrlLeftJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::CtrlLeft); }
inline bool Input::ctrlLeftJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::CtrlLeft); }
inline bool Input::ctrlRightPressed() const { return m_impl->isPressed(impl::SpecialKey::CtrlRight); }
inline bool Input::ctrlRightJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::CtrlRight); }
inline bool Input::ctrlRightJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::CtrlRight); }
inline bool Input::altLeftPressed() const { return m_impl->isPressed(impl::SpecialKey::AltLeft); }
inline bool Input::altLeftJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::AltLeft); }
inline bool Input::altLeftJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::AltLeft); }
inline bool Input::altRightPressed() const { return m_impl->isPressed(impl::SpecialKey::AltRight); }
inline bool Input::altRightJustPressed() const { return m_impl->isJustPressed(impl::SpecialKey::AltRight); }
inline bool Input::altRightJustOutpressed() const { return m_impl->isJustOutpressed(impl::SpecialKey::AltRight); }

}
