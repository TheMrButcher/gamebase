/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/Layout.h>
#include <gamebase/ui/TextBox.h>
#include <gamebase/ui/ComboBox.h>
#include <gamebase/ui/CheckBox.h>
#include <gamebase/ui/Button.h>
#include <gamebase/drawobj/Label.h>

namespace gamebase { namespace editor {
Layout createPropertyLayout();
Layout createNameLayout();
Label createLabel(const std::string& text);
Label createRightLabel(const std::string& text);
TextBox createTextBox();
ComboBox createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices = std::vector<int>());
CheckBox createCheckBox();
Layout createSpacer();
Button createColorRect();

TextBox createResourcePathTextBox();
Button createChoosePathButton();
} }
