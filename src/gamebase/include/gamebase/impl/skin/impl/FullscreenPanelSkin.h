/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/PanelSkin.h>
#include <gamebase/impl/drawobj/FilledRect.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FullscreenPanelSkin : public PanelSkin, public ISerializable {
public:
    FullscreenPanelSkin(const Color& bgColor) { setBackgroundColor(bgColor); }

    Color backgroundColor() const { return m_background.color(); }
    void setBackgroundColor(const Color& bgColor) { m_background.setColor(bgColor); }

    virtual std::shared_ptr<Button> createCloseButton() const override { return nullptr; }
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const override { return nullptr; }

    virtual BoundingBox panelBox() const
    {
        return m_background.box();
    }

    virtual void loadResources() override
    {
        m_background.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_background.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_background.setBox(allowedBox);
    }

    virtual BoundingBox box() const override
    {
        return m_background.box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

private:
    FilledRect m_background;
};

} }
