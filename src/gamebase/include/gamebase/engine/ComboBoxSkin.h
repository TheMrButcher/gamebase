#pragma once

#include <gamebase/engine/Skin.h>
#include <gamebase/engine/ToggleButton.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/engine/ButtonList.h>

namespace gamebase {

class ComboBoxSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<ToggleButton> createOpenButton() const = 0;

    virtual std::shared_ptr<TextBox> createTextBox() const = 0;

    virtual std::shared_ptr<ButtonList> createList() const = 0;
};

}
