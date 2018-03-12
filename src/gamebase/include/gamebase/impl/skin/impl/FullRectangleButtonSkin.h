/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/impl/SimpleRectangleButtonSkin.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FullRectangleButtonSkin : public SimpleRectangleButtonSkin {
public:
    FullRectangleButtonSkin(const std::shared_ptr<IRelativeBox>& box);

    void setContentPositioning(float padding, Direction::Enum direction);
    void setImageName(const std::string& path) { m_icon.setImageName(path); }
    void setImageSizes(float width, float height) { m_icon.setFixedBox(width, height); }
    void setImageColor(const GLColor& color) { m_icon.setColor(color); }
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    virtual void serialize(Serializer& s) const override;

protected:
    virtual void loadContent() override;
    virtual void drawContent(const Transform2& position) const override;
    virtual void setContentBox(const BoundingBox& innerBox) override;
    
    float m_padding;
    Direction::Enum m_direction;
    StaticTextureRect m_icon;
};

} }
