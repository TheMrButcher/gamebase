/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "ColorDialog.h"
#include <dvb/Helpers.h>
#include <gamebase/serial/LoadObj.h>
#include <gamebase/text/StringUtils.h>
#include <gamebase/tools/Connect.h>

namespace gamebase { namespace editor {

class IColorFormatter {
public:
    virtual ~IColorFormatter() {}

    virtual std::string toHumanReadable(float color) const = 0;

    virtual float toGL(const std::string& colorStr) const = 0;
};

namespace {
int toIntColor(float color)
{
	return static_cast<int>(color * 255 + 0.5f);
}

float toGLColor(int color)
{
	if (color < 0)
		return 0;
	return color / 255.0f;
}

class ColorFormatter255 : public IColorFormatter {
public:
    virtual std::string toHumanReadable(float color) const override
    {
        return toString(toIntColor(color));
    }

    virtual float toGL(const std::string& colorStr) const override
    {
        return toGLColor(toInt(colorStr));
    }
};

class ColorFormatterGL : public IColorFormatter {
public:
    virtual std::string toHumanReadable(float color) const override
    {
        return toUIString(color);
    }

    virtual float toGL(const std::string& colorStr) const override
    {
        return toFloat(colorStr);
    }
};

const ColorFormatter255 FORMAT_255;
const ColorFormatterGL FORMAT_GL;

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

void ColorDialog::attachPanel(Panel panel)
{
	m_panel = panel;
	m_color = impl::GLColor(0, 0, 0, 0);
	m_colorRect = panel.child<FilledRect>("colorRect");
	m_redBox = panel.child<TextBox>("redBox");
	m_greenBox = panel.child<TextBox>("greenBox");
	m_blueBox = panel.child<TextBox>("blueBox");
	m_alphaBox = panel.child<TextBox>("alphaBox");
	m_redSlider = panel.child<ScrollBar>("redSlider");
	m_greenSlider = panel.child<ScrollBar>("greenSlider");
	m_blueSlider = panel.child<ScrollBar>("blueSlider");
	m_alphaSlider = panel.child<ScrollBar>("alphaSlider");
	m_switchFormat = panel.child<ToggleButton>("switchFormat");
	m_ok = panel.child<Button>("ok");
	m_cancel = panel.child<Button>("cancel");
	m_curFormat = &FORMAT_255;

	m_cancel.setCallback([this]() { m_panel.hide(); });
	m_redBox.setCallback([this]() { colorFromTextBoxes(); });
	m_greenBox.setCallback([this]() { colorFromTextBoxes(); });
	m_blueBox.setCallback([this]() { colorFromTextBoxes(); });
	m_alphaBox.setCallback([this]() { colorFromTextBoxes(); });
	connect(m_panel.child<Button>("redInc"), changeComponent, &m_color.r, 1);
	connect(m_panel.child<Button>("redDec"), changeComponent, &m_color.r, -1);
	connect(m_panel.child<Button>("greenInc"), changeComponent, &m_color.g, 1);
	connect(m_panel.child<Button>("greenDec"), changeComponent, &m_color.g, -1);
	connect(m_panel.child<Button>("blueInc"), changeComponent, &m_color.b, 1);
	connect(m_panel.child<Button>("blueDec"), changeComponent, &m_color.b, -1);
	connect(m_panel.child<Button>("alphaInc"), changeComponent, &m_color.a, 1);
	connect(m_panel.child<Button>("alphaDec"), changeComponent, &m_color.a, -1);

	std::function<void()> updateFromSliders = [this]() { colorFromSliders(); };
	m_redSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.r, updateFromSliders));
	m_greenSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.g, updateFromSliders));
	m_blueSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.b, updateFromSliders));
	m_alphaSlider.getImpl()->setControlledValue(std::make_shared<ColorComponent>(&m_color.a, updateFromSliders));

	connect(m_switchFormat, switchColorFormat);
    
    fillPalette(m_panel.child<Layout>("paletteTopRow"));
    fillPalette(m_panel.child<Layout>("paletteBottomRow"));
	connect(m_panel.child<Button>("fromPalette"), fromPalette);
	connect(m_panel.child<Button>("toPalette"), toPalette);
    m_panel.child<Layout>("canvas").update();

	update();
}

void ColorDialog::setColor(const Color& color)
{
	m_color = impl::makeGLColor(color);
	update();
}

void ColorDialog::processResult(const std::function<void(const Color&)>& callback)
{
	if (callback)
		callback(m_color.intColor());
	m_panel.hide();
}

void ColorDialog::colorFromTextBoxes()
{
	try {
		m_color.r = m_curFormat->toGL(m_redBox.text());
		m_color.g = m_curFormat->toGL(m_greenBox.text());
		m_color.b = m_curFormat->toGL(m_blueBox.text());
		m_color.a = m_curFormat->toGL(m_alphaBox.text());
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
	*component = toGLColor(toIntColor(*component) + delta);
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
	m_colorRect.setColor(m_color.intColor());
}

void ColorDialog::updateTextBoxes()
{
	m_redBox.setText(m_curFormat->toHumanReadable(m_color.r));
	m_greenBox.setText(m_curFormat->toHumanReadable(m_color.g));
	m_blueBox.setText(m_curFormat->toHumanReadable(m_color.b));
	m_alphaBox.setText(m_curFormat->toHumanReadable(m_color.a));
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

void ColorDialog::switchColorFormat()
{
    try {
		m_color.r = m_curFormat->toGL(m_redBox.text());
		m_color.g = m_curFormat->toGL(m_greenBox.text());
		m_color.b = m_curFormat->toGL(m_blueBox.text());
		m_color.a = m_curFormat->toGL(m_alphaBox.text());

        if (m_switchFormat.isPressed())
            m_curFormat = &FORMAT_GL;
        else
            m_curFormat = &FORMAT_255;

        updateTextBoxes();
	} catch (std::exception& ex) {
		std::cerr << "Error while reading color from text boxes. Reason: " << ex.what() << std::endl;
	}
	update();
}

void ColorDialog::fillPalette(Layout layout)
{
    for (int i = 0; i < 5; ++i) {
        auto button = loadObj<RadioButton>("ui\\ColorRadioButton.json");
        layout.add(button);
        m_palette.add(button);
        if (!m_palette.isAnySelected())
            m_palette.select(button);
    }
}

void ColorDialog::toPalette()
{
    auto button = m_palette.selectedButton();
    auto colorRect = button.child<FilledRect>("colorRect");
    colorRect.setColor(m_color.intColor());
}

void ColorDialog::fromPalette()
{
    auto button = m_palette.selectedButton();
    auto colorRect = button.child<FilledRect>("colorRect");
    m_color = impl::makeGLColor(colorRect.color());
    update();
}

ColorDialog& getColorDialog()
{
    static ColorDialog dialog;
    return dialog;
}

} }
