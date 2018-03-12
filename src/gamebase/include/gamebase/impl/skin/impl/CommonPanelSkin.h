/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/PanelSkin.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CommonPanelSkin : public PanelSkin, public ISerializable {
public:
    CommonPanelSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeBox>& panelBox);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setIsLimitedByBox(bool value) { m_isLimitedByBox = value; }
    void setTransparent(bool value) { m_transparent = value; }

    void setCloseButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setDragBarSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual std::shared_ptr<Button> createCloseButton() const override;
    virtual std::shared_ptr<ScrollDragBar> createDragBar() const override;

    virtual bool isLimitedByBox() const { return m_isLimitedByBox; }
    virtual bool transparent() const { return m_transparent; }
    virtual BoundingBox panelBox() const { return m_panelBox->get(); }

    virtual void loadResources() override
    {
        m_skinElements.loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_skinElements.draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_box->setParentBox(allowedBox);
        m_panelBox->setParentBox(box());
        m_skinElements.setBox(box());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_panelBox;

    std::shared_ptr<ButtonSkin> m_closeButtonSkin;
    std::shared_ptr<IRelativeOffset> m_closeButtonPosition;
    std::shared_ptr<ButtonSkin> m_dragBarSkin;
    std::shared_ptr<IRelativeOffset> m_dragBarPosition;

    bool m_isLimitedByBox;
    bool m_transparent;

    ObjectsCollection m_skinElements;
};

} }
