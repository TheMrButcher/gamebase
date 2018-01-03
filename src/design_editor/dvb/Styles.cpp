/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Styles.h"
#include <tools.h>
#include <gamebase/serial/LoadObj.h>

namespace gamebase { namespace editor {
Layout createPropertyLayout()
{
    return loadObj<Layout>("ui\\PropertyLayout.json");
}

Layout createNameLayout()
{
    return loadObj<Layout>("ui\\PropertyNameLayout.json");
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

namespace {
bool lessByKey(const std::pair<std::string, int>& p1, const std::pair<std::string, int>& p2)
{
	return p1.first < p2.first;
}

bool equalByKey(const std::pair<std::string, int>& p1, const std::pair<std::string, int>& p2)
{
	return p1.first == p2.first;
}
}

ComboBox createComboBox(
    const std::vector<std::string>& variants,
    const std::vector<int>& indices)
{
	std::vector<std::pair<std::string, int>> variantsWithIndices;
	auto itVariant = variants.begin();
    auto itIndex = indices.begin();
	for (int i = 0; itVariant != variants.end(); ++itVariant, ++i) {
        const auto& text = *itVariant;
		if (itIndex == indices.end())
            variantsWithIndices.push_back(std::make_pair(text, i));
        else
            variantsWithIndices.push_back(std::make_pair(text, *itIndex++));
	}

	std::sort(variantsWithIndices.begin(), variantsWithIndices.end(), &lessByKey);
	variantsWithIndices.erase(
		std::unique(variantsWithIndices.begin(), variantsWithIndices.end(), &equalByKey),
		variantsWithIndices.end());

    auto comboBox = loadObj<ComboBox>("ui\\PropertyComboBox.json");
    for (auto itVariant = variantsWithIndices.begin(); itVariant != variantsWithIndices.end(); ++itVariant) {
        const auto& text = itVariant->first;
		auto index = itVariant->second;
        auto button = loadObj<Button>("ui\\ComboBoxButton.json");
        button.child<Label>("label").setText(text);
        comboBox.insert(index, text, button);
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

Button createColorRect()
{
	return loadObj<Button>("ui\\ColorRect.json");
}

TextBox createResourcePathTextBox()
{
	return loadObj<TextBox>("ui\\ResourcePathTextBox.json");
}

Button createChoosePathButton()
{
	return loadObj<Button>("ui\\ChoosePathButton.json");
}
} }

