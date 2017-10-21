/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ScrollableAreaSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class BaseSimpleButtonSkin;

class GAMEBASE_API SimpleScrollableAreaSkin : public ScrollableAreaSkin, public ISerializable {
public:
    SimpleScrollableAreaSkin(const std::shared_ptr<IRelativeBox>& box);

    void setScrollBarEnabled(Direction::Enum direction, bool value) { m_scrollBarEnabled[direction] = value; }
    void setScrollBarWidth(float width) { m_scrollBarWidth = width; }
    void setScrollStep(float value) { m_scrollStep = value; }
    void setBackgroundColor(const GLColor& color) { m_backgroundColor = color; }
	void setBorderWidth(float width) { m_borderWidth = width; }
	void setBorderColor(const GLColor& color) { m_borderColor = color; }
	void setUseGradient(bool value) { m_useGradient = value; }
	void setFillColor(const GLColor& color) { m_fillColor = color; }
    void setArrowPadding(float padding) { m_arrowPadding = padding; }
    void setArrowColor(const GLColor& color) { m_arrowColor = color; }

    virtual std::shared_ptr<ScrollBar> createScrollBar(
        const std::shared_ptr<FloatValue>& controlledValue,
        Direction::Enum direction) const override;

    virtual BoundingBox areaBox() const override { return m_curAreaBox; }
    virtual void setSize(float width, float height) override;
    virtual void setFixedBox(float width, float height) override;

    virtual void loadResources() override {}
    virtual void drawAt(const Transform2& position) const override {}
    
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer& s) const override;

protected:
    void fillSkin(BaseSimpleButtonSkin* skin) const;

    std::shared_ptr<IRelativeBox> m_box;
    bool m_scrollBarEnabled[2];
    float m_scrollBarWidth;
    float m_scrollStep;
    GLColor m_backgroundColor;
    float m_borderWidth;
    GLColor m_borderColor;
    bool m_useGradient;
    GLColor m_fillColor;
    float m_arrowPadding;
    GLColor m_arrowColor;

    BoundingBox m_curAreaBox;
};

} }
