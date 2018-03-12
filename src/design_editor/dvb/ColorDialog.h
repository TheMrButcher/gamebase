/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/ui/Panel.h>
#include <gamebase/ui/TextBox.h>
#include <gamebase/ui/ScrollBar.h>
#include <gamebase/ui/ToggleButton.h>
#include <gamebase/ui/Button.h>
#include <gamebase/ui/RadioGroup.h>
#include <gamebase/drawobj/FilledRect.h>

namespace gamebase { namespace editor {

class IColorFormatter;

class ColorDialog {
public:
    ColorDialog() {}

    void attachPanel(Panel panel);

	Color color() const { return m_color.intColor(); }
	void setColor(const Color& color);
	void showWithColor(
		const Color& color,
		const std::function<void(const Color&)>& okCallback)
	{
		setColor(color);
		setCallbacks(okCallback);
		show();
	}

    void setCallbacks(
        const std::function<void(const Color&)>& okCallback,
        const std::function<void()>& cancelCallback = nullptr)
    {
        m_panel.setCallback(cancelCallback);
		m_ok.setCallback([this, okCallback]() { processResult(okCallback); });
    }

    void show()
    {
        m_panel.show();
    }

private:
    void processResult(const std::function<void(const Color&)>& callback);
	void colorFromTextBoxes();
	void colorFromSliders();
	void changeComponent(float* component, int delta);
	void update();
	void updateColorRect();
	void updateTextBoxes();
	void updateSliders();
    void switchColorFormat();
    void fillPalette(Layout layout);
    void toPalette();
    void fromPalette();

    Panel m_panel;
	impl::GLColor m_color;
	FilledRect m_colorRect;
	TextBox m_redBox;
	TextBox m_greenBox;
	TextBox m_blueBox;
	TextBox m_alphaBox;
	ScrollBar m_redSlider;
	ScrollBar m_greenSlider;
	ScrollBar m_blueSlider;
	ScrollBar m_alphaSlider;
    ToggleButton m_switchFormat;
    const IColorFormatter* m_curFormat;
    RadioGroup m_palette;
    Button m_ok;
    Button m_cancel;
};

ColorDialog& getColorDialog();

} }
