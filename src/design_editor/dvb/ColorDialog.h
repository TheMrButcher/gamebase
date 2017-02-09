/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/Gamebase.h>

namespace gamebase { namespace editor {

class IColorFormatter;

class ColorDialog {
public:
    ColorDialog() {}

    ColorDialog(Panel panel);

	void init();

	Color color() const { return m_color; }
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
        m_ok.setCallback(std::bind(&ColorDialog::processResult, this, okCallback));
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

    Panel m_panel;
	Color m_color;
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
    Button m_ok;
    Button m_cancel;
};

ColorDialog& getColorDialog();

} }
