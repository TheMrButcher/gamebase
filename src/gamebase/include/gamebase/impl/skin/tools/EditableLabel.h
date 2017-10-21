/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/text/TextGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <algorithm>

namespace gamebase { namespace impl {

class GAMEBASE_API EditableLabel : public OffsettedPosition, public Drawable,
    public Registrable, public ISerializable {
public:
    EditableLabel(const std::shared_ptr<IRelativeBox>& box);

    void setRelativeBox(const std::shared_ptr<IRelativeBox>& box) { m_box = box; }

	const std::string& text() const { return m_text; }
    void setText(const std::string& text);

	const FontDesc& font() const { return m_alignProps.font; }
    void setFont(const FontDesc& fontDesc) { m_alignProps.font = fontDesc; }
	
	const GLColor& color() const { return m_color; }
    void setColor(const GLColor& color) { m_color = color; }
    
	const GLColor& selectionColor() const { return m_selectionRect.color(); }
	void setSelectionColor(const GLColor& color) { m_selectionRect.setColor(color); }
    
	void setSelection(size_t startIndex, size_t endIndex)
    {
        m_selection = std::minmax(startIndex, endIndex);
    }
    void setLimited(bool value) { m_isLimited = value; }
    void setOffsetX(float x) { m_offsetX = x; }

    const std::vector<CharPosition>& textGeometry() const { return m_textGeom; }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
    }

    virtual BoundingBox box() const override { return m_box->get(); }
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override
    {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    }
    
    virtual void serialize(Serializer& s) const override;

private:
    void updateTextGeometry();

    std::shared_ptr<IRelativeBox> m_box;

    FilledRect m_selectionRect;
    GLBuffers m_buffers;
    std::shared_ptr<IFont> m_font;
    std::vector<CharPosition> m_textGeom;
    float m_offsetX;
    std::vector<CharPosition> m_visibleTextGeom;
    
    std::string m_text;
    AlignProperties m_alignProps;
    GLColor m_color;
    std::pair<size_t, size_t> m_selection;
    bool m_isLimited;
};

} }
