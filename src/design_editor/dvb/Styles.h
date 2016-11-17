/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/drawobj/Label.h>
#include <gamebase/ui/Layout.h>
#include <gamebase/ui/TextBox.h>
#include <gamebase/ui/ComboBox.h>

namespace gamebase { namespace editor {
Layout createPropertyLayout();
Label createLabel(const std::string& text);
Label createRightLabel(const std::string& text);
TextBox createTextBox();
ComboBox createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices = std::vector<int>());
} }
