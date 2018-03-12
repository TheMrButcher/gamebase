/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/impl/AnimatedCheckBoxSkin.h>
#include <gamebase/impl/skin/tools/AnimatedObject.h>
#include <gamebase/impl/drawobj/StaticLabel.h>
#include <boost/optional.hpp>

namespace gamebase { namespace impl {

class GAMEBASE_API ClickableTextCheckBoxSkin : public AnimatedCheckBoxSkin {
public:
    ClickableTextCheckBoxSkin(
        const std::shared_ptr<IRelativeBox>& labelBox,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr);

    void setText(const std::string& text);

    void setTextColor(const GLColor& color)
    {
        m_label.setColor(color);
    }

    void setFont(const FontDesc& fontDesc)
    {
        AlignProperties alignProps = m_label.alignProperties();
        alignProps.font = fontDesc;
        m_label.setAlignProperties(alignProps);
    }

    virtual void drawAt(const Transform2& position) const override
    {
        AnimatedCheckBoxSkin::drawAt(position);
        m_label.draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_label.setBox(allowedBox);
        AnimatedCheckBoxSkin::setBox(m_label.box());
    }

    virtual void loadResources() override;
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_labelBox;
    StaticLabel m_label;
};

} }
