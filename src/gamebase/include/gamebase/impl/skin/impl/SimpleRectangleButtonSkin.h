/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ButtonSkin.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/drawobj/Gradient.h>
#include <gamebase/impl/drawobj/StaticLabel.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleRectangleButtonSkin : public ButtonSkin, public ISerializable {
public:
    SimpleRectangleButtonSkin(const std::shared_ptr<IRelativeBox>& box);

	void setBorderWidth(float width) { m_borderWidth = width; }
	void setBorderColor(const Color& color) { m_border.setColor(color); }

	void setUseGradient(bool value) { m_useGradient = value; }
	void setFillColor(const Color& color) { m_fill.setColor(color); }

	void setFont(const FontDesc& font);
	void setText(const std::string& text) { m_label.setText(text); }
	void setTextColor(const Color& color) { m_label.setColor(color); }
    
	virtual std::shared_ptr<IRelativeGeometry> geometry() const override { return m_geom; }

	virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_box;
	std::shared_ptr<IRelativeGeometry> m_geom;

	float m_borderWidth;
	bool m_useGradient;

	FilledRect m_border;
	FilledRect m_fill;
	Gradient m_gradient;
	FilledRect m_light;
	StaticLabel m_label;
};

} }
