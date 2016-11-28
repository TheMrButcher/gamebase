/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Styles.h"
#include <tools.h>

namespace gamebase { namespace editor {
Layout createPropertyLayout()
{
    return loadObj<Layout>("ui\\PropertyLayout.json");
}

Label createLabel(const std::string& text)
{
    auto label = loadObj<Label>("ui\\PropertyLabel.json");
    label.setText(text + ": ");
    return label;
}

Label createRightLabel(const std::string& text)
{
    auto label = loadObj<Label>("ui\\PropertyRightLabel.json");
    label.setText(text);
    return label;
}

TextBox createTextBox()
{
    return loadObj<TextBox>("ui\\PropertyTextBox.json");
}

ComboBox createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices)
{
    auto comboBox = loadObj<ComboBox>("ui\\PropertyComboBox.json");
    auto itIndex = indices.begin();
    for (auto itVariant = variants.begin(); itVariant != variants.end(); ++itVariant) {
        const auto& text = *itVariant;
        auto button = loadObj<Button>("ui\\ComboBoxButton.json");
        button.child<Label>("label").setText(text);
        if (itIndex == indices.end())
            comboBox.add(text, button);
        else
            comboBox.insert(*itIndex++, text, button);
    }
    return comboBox;
}

CheckBox createCheckBox()
{
    return loadObj<CheckBox>("ui\\CheckBox.json");
}

Layout createSpacer()
{
    return loadObj<Layout>("ui\\PropertySpacer.json");
}
} }

