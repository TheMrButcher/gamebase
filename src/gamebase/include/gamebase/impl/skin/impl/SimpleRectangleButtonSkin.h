/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/tools/BaseSimpleButtonSkin.h>
#include <gamebase/impl/drawobj/StaticLabel.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleRectangleButtonSkin : public BaseSimpleButtonSkin, public ISerializable {
public:
    SimpleRectangleButtonSkin(const std::shared_ptr<IRelativeBox>& box);

	void setFont(const FontDesc& font);
	void setText(const std::string& text) { m_label.setText(text); }
	void setTextColor(const GLColor& color) { m_label.setColor(color); }
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

protected:
    virtual void loadContent() override;
    virtual void drawContent(const Transform2& position) const override;
    virtual void setContentBox(const BoundingBox& innerBox) override;

	StaticLabel m_label;
};

} }
