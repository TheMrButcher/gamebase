/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ButtonSkin.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/drawobj/Gradient.h>

namespace gamebase { namespace impl {

class GAMEBASE_API BaseSimpleButtonSkin : public ButtonSkin {
public:
    BaseSimpleButtonSkin(const std::shared_ptr<IRelativeBox>& box);

    void setBorderWidth(float width) { m_borderWidth = width; }
    void setBorderColor(const GLColor& color) { m_border.setColor(color); }

    void setUseGradient(bool value) { m_useGradient = value; }
    void setFillColor(const GLColor& color) { m_fill.setColor(color); }
    
    virtual std::shared_ptr<IRelativeGeometry> geometry() const override { return m_geom; }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

protected:
    void serializeParams(Serializer& s) const;
    virtual void loadContent() = 0;
    virtual void drawContent(const Transform2& position) const = 0;
    virtual void setContentBox(const BoundingBox& innerBox) = 0;

    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeGeometry> m_geom;

    float m_borderWidth;
    bool m_useGradient;

    FilledRect m_border;
    FilledRect m_fill;
    Gradient m_gradient;
    FilledRect m_light;
};

void deserializeBaseSimpleButtonSkin(
    BaseSimpleButtonSkin* skin, Deserializer& deserializer);

} }
