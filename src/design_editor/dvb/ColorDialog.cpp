/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "ColorDialog.h"

namespace gamebase { namespace editor {

namespace {
int toIntColor(float color)
{
	return static_cast<int>(color * 255 + 0.5f);
}

std::string toHumanReadable(float color)
{
	return toString(toIntColor(color));
}

float toGL(int color)
{
	if (color < 0)
		return 0;
	return color / 255.0f;
}

float toGL(const std::string& colorStr)
{
	return toGL(toInt(colorStr));
}

class ColorComponent : public impl::FloatValue {
public:
	ColorComponent(
		float* component,
		const std::function<void()>& updateCallback)
		: m_component(component)
		, m_updateCallback(updateCallback)
	{}

	float get() const { return *m_component > 1 ? 1 : *m_component; }
	void set(const float& value)
	{
		*m_component = value;
		m_updateCallback();
	}

private:
	float* m_component;
	std::function<void()> m_updateCallback;
};
}

ColorDialog::ColorDialog(Panel panel)
    : m_panel(panel)
	, m_color(Color(0, 0, 0, 0))
	, m_colorRect(panel.child<FilledRect>("colorRect"))
	, m_redBox(panel.child<TextBox>("redBox"))
    , m_greenBox(panel.child<TextBox>("greenBox"))
    , m_blueBox(panel.child<TextBox>("blueBox"))
    , m_alphaBox(panel.child<TextBox>("alphaBox"))
	, m_redSlider(panel.child<ScrollBar>("redSlider"))
    , m_greenSlider(panel.child<ScrollBar>("greenSlider"))
    , m_blueSlider(panel.child<ScrollBar>("blueSlider"))
    , m_alphaSlider(panel.child<ScrollBar>("alphaSlider"))
    , m_ok(panel.child<Button>("ok"))
    , m_cancel(panel.child<Button>("cancel"))
{
}

void ColorDialog::init()
{
    m_cancel.setCallback(std::bind(&Panel::hide, m_panel));
	m_redBox.setCallback(std::bind(&ColorDialog::colorFromTextBoxes, this));
	m_greenBox.setCallback(std::bind(&ColorDialog::colorFromTextBoxes, this));
	m_blueBox.setCallback(std::bind(&ColorDialog::colorFromTextBoxes, this));
	m_alphaBox.setCallback(std::bind(&ColorDialog::colorFromTextBoxes, this));
	m_panel.child<Button>("redInc").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.r, 1));
	m_panel.child<Button>("redDec").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.r, -1));
	m_panel.child<Button>("greenInc").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.g, 1));
	m_panel.child<Button>("greenDec").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.g, -1));
	m_panel.child<Button>("blueInc").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.b, 1));
	m_panel.child<Button>("blueDec").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.b, -1));
	m_panel.child<Button>("alphaInc").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.a, 1));
	m_panel.child<Button>("alphaDec").setCallback(std::bind(&ColorDialog::changeComponent, this, &m_color.a, -1));

	std::function<void()> updateFromSliders = std::bind(&ColorDialog::colorFromSliders, this);
	m_redSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.r, updateFromSliders));
	m_greenSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.g, updateFromSliders));
	m_blueSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.b, updateFromSliders));
	m_alphaSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.a, updateFromSliders));

	update();
}

void ColorDialog::setColor(const Color& color)
{
	m_color = color;
	update();
}

void ColorDialog::processResult(const std::function<void(const Color&)>& callback)
{
	if (callback)
		callback(m_color);
	m_panel.hide();
}

void ColorDialog::colorFromTextBoxes()
{
	try {
		m_color.r = toGL(m_redBox.text());
		m_color.g = toGL(m_greenBox.text());
		m_color.b = toGL(m_blueBox.text());
		m_color.a = toGL(m_alphaBox.text());
	} catch (std::exception& ex) {
		std::cerr << "Error while reading color from text boxes. Reason: " << ex.what() << std::endl;
	}
	update();
}

void ColorDialog::colorFromSliders()
{
	updateColorRect();
	updateTextBoxes();
}

void ColorDialog::changeComponent(float* component, int delta)
{
	*component = toGL(toIntColor(*component) + delta);
	update();
}

void ColorDialog::update()
{
	updateSliders();
	updateColorRect();
	updateTextBoxes();
}

void ColorDialog::updateColorRect()
{
	m_colorRect.setColor(m_color);
}

void ColorDialog::updateTextBoxes()
{
	m_redBox.setText(toHumanReadable(m_color.r));
	m_greenBox.setText(toHumanReadable(m_color.g));
	m_blueBox.setText(toHumanReadable(m_color.b));
	m_alphaBox.setText(toHumanReadable(m_color.a));
}

void ColorDialog::updateSliders()
{
	auto color = m_color;
	m_redSlider.update();
	m_greenSlider.update();
	m_blueSlider.update();
	m_alphaSlider.update();
	m_color = color;
}

ColorDialog& getColorDialog()
{
    static ColorDialog dialog;
    return dialog;
}

} }
