#pragma once

#include <gamebase/engine/Selectable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/serial/ISerializable.h>
#include <functional>

namespace gamebase {

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

}
