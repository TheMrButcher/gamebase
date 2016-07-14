/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/Selectable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <functional>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;

class SelectableElement : public Selectable, public ISerializable, public Registrable {
public:
    virtual void setCallback(const std::function<void()>& callback) = 0;

    virtual void setTransitionCallback(
        const std::function<void(SelectionState::Enum)>& callback) = 0;

    virtual void step() = 0;

    virtual bool isMouseOn() const = 0;

    virtual bool isPressed() const = 0;

    virtual bool isClicked() const = 0;
};

} }
