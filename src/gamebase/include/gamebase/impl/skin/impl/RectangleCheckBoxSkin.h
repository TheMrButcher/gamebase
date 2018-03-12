/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/tools/BaseCheckBoxSkin.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API RectangleCheckBoxSkin : public BaseCheckBoxSkin, public ISerializable {
public:
    RectangleCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box);

    void setBorderWidth(float width) { m_borderWidth = width; }
    void setBorderColor(const GLColor& color) { m_border.setColor(color); }
    void setFillColor(const GLColor& color) { m_fill.setColor(color); }
    
    void setCheckImageName(const std::string& path) { m_check.setImageName(path); }
    void setCheckImageSizes(float width, float height) { m_check.setFixedBox(width, height); }
    void setCheckImageColor(const GLColor& color) { m_check.setColor(color); }

    virtual void loadResources() override;
    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& s) const override;

protected:
    virtual void drawBackground(const Transform2& position) const override;
    virtual void drawCheck(const Transform2& position) const override;
    virtual void setContentBox(const BoundingBox& box) override;

    float m_borderWidth;

    FilledRect m_border;
    FilledRect m_fill;
    StaticTextureRect m_check;
};

} }
