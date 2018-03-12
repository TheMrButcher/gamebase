/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/Skin.h>
#include <gamebase/impl/ui/ToggleButton.h>
#include <gamebase/impl/ui/TextBox.h>
#include <gamebase/impl/ui/ButtonList.h>

namespace gamebase { namespace impl {

class ComboBoxSkin : public Drawable, public Registrable {
public:
    virtual std::shared_ptr<ToggleButton> createOpenButton() const = 0;

    virtual std::shared_ptr<TextBox> createTextBox() const = 0;

    virtual std::shared_ptr<ButtonList> createList() const = 0;
};

} }
