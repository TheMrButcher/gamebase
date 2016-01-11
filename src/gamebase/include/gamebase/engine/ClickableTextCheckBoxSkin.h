#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/AnimatedCheckBoxSkin.h>
#include <gamebase/engine/AnimatedObject.h>
#include <gamebase/engine/StaticLabel.h>
#include <boost/optional.hpp>

namespace gamebase {

class GAMEBASE_API ClickableTextCheckBoxSkin : public AnimatedCheckBoxSkin {
public:
    ClickableTextCheckBoxSkin(
        const std::shared_ptr<IRelativeBox>& labelBox,
        const std::shared_ptr<IRelativeBox>& extBox,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr);

    void setText(const std::string& text)
    {
        m_label.setText(text);
    }

    void setTextColor(const Color& color)
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
        m_parentBox = allowedBox;
        m_label.setBox(allowedBox);
        AnimatedCheckBoxSkin::setBox(m_label.box());
    }

    virtual void loadResources() override;
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

private:
    boost::optional<BoundingBox> m_parentBox;
    std::shared_ptr<IRelativeBox> m_labelBox;
    StaticLabel m_label;
};

}
