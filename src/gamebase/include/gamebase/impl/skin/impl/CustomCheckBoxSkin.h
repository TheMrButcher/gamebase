/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/tools/BaseCheckBoxSkin.h>
#include <gamebase/impl/drawobj/StaticTextureRect.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CustomCheckBoxSkin : public BaseCheckBoxSkin, public ISerializable {
public:
    CustomCheckBoxSkin(const std::shared_ptr<IRelativeBox>& box);

    void setBackgroundImageName(const std::string& path) { m_background.setImageName(path); }
    void setBackgroundImageColor(const GLColor& color) { m_background.setColor(color); }

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

    StaticTextureRect m_background;
    StaticTextureRect m_check;
};

} }
