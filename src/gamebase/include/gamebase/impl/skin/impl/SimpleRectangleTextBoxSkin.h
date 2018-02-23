/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/TextBoxSkin.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/skin/tools/EditableLabel.h>
#include <gamebase/impl/skin/tools/TextBoxCursor.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleRectangleTextBoxSkin : public TextBoxSkin, public ISerializable {
public:
    SimpleRectangleTextBoxSkin(const std::shared_ptr<IRelativeBox>& box);

    void setBorderWidth(float width) { m_borderWidth = width; }
    void setBorderColor(const GLColor& color) { m_border.setColor(color); }

    void setFillColor(const GLColor& color) { m_fill.setColor(color); }

    void setFont(const FontDesc& font) { m_label.setFont(font); }
    void setTextColor(const GLColor& color);
    void setTextOutlineColor(const GLColor& color);
    void setSelectionColor(const GLColor& color) { m_label.setSelectionColor(color); }

    virtual void setText(const std::string& text) override
    {
        m_label.setText(text);
    }

    virtual void setSelection(size_t startIndex, size_t endIndex) override;

    virtual const std::vector<CharPosition>& textGeometry() const override
    {
        return m_label.textGeometry();
    }

    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual Time shiftPeriod() const override
    {
        return 100;
    }

    virtual void setOffsetX(float x)
    {
        m_cursorOffsetX = x;
        m_label.setOffsetX(x);
    }
    
    virtual BoundingBox textAreaBox() const override
    {
        return m_label.box();
    }

    virtual void setSelectionState(SelectionState::Enum state) override
    {
        m_selectionState = state;
    }

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeGeometry> m_geom;
    
    float m_borderWidth;
    float m_cursorOffsetX;
    size_t m_cursorPos;
    bool m_drawCursor;
    bool m_loaded;
    
    FilledRect m_border;
    FilledRect m_fill;
    EditableLabel m_label;
    TextBoxCursor m_cursor;
};

} }
