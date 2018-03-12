/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/drawobj/Gradient.h>
#include <gamebase/impl/drawobj/StaticLabel.h>
#include <gamebase/impl/relbox/RelativeBox.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleToolTipSkin : public Drawable, public Registrable, public ISerializable {
public:
    SimpleToolTipSkin(const RelativeValue& maxWidth);

    void setBorderWidth(float width) { m_borderWidth = width; }
    void setBorderColor(const GLColor& color) { m_border.setColor(color); }

    void setPadding(float padding) { m_padding = padding; }
    void setFillColor(const GLColor& color) { m_fill.setColor(color); }

    void setFont(const FontDesc& font);
    void setText(const std::string& text) { m_label.setText(text); }
    void setEnableStacking(bool enableStacking);
    void setTextColor(const GLColor& color) { m_label.setColor(color); }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_border.box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<RelativeBox> m_box;

    float m_borderWidth;
    float m_padding;

    FilledRect m_border;
    FilledRect m_fill;
    StaticLabel m_label;
};

} }
