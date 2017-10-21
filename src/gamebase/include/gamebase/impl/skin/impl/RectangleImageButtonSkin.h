/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/tools/BaseSimpleButtonSkin.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API RectangleImageButtonSkin : public BaseSimpleButtonSkin, public ISerializable {
public:
    RectangleImageButtonSkin(const std::shared_ptr<IRelativeBox>& box);

    void setImageName(const std::string& path) { m_icon.setImageName(path); }
    void setImageSizes(float width, float height) { m_icon.setFixedBox(width, height); }
    void setImageColor(const GLColor& color) { m_icon.setColor(color); }
    
    virtual void registerObject(PropertiesRegisterBuilder*) override;
    virtual void serialize(Serializer& s) const override;

protected:
    virtual void loadContent() override;
    virtual void drawContent(const Transform2& position) const override;
    virtual void setContentBox(const BoundingBox& innerBox) override;

    StaticTextureRect m_icon;
};

} }
