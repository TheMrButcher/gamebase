/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ScrollBarSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CommonScrollBarSkin : public ScrollBarSkin, public ISerializable {
public:
    CommonScrollBarSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeBox>& dragBox,
        Direction::Enum direction);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setAlwaysShow(bool value) { m_alwaysShow = value; }
    void setStep(float value) { m_step = value; }

    void setDecButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setIncButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setDragBarSkin(const std::shared_ptr<ButtonSkin>& skin)
    {
        m_dragBarSkin = skin;
    }

    virtual std::shared_ptr<Button> createDecButton() const override;
    virtual std::shared_ptr<Button> createIncButton() const override;

    virtual std::shared_ptr<ScrollDragBar> createDragBar(
        const std::shared_ptr<FixedOffset>& position) const override;

    virtual BoundingBox dragBox() const override { return m_dragBox->get(); }
    virtual Direction::Enum direction() const override { return m_direction; }
    virtual bool alwaysShow() const override { return m_alwaysShow; }
    virtual float step() const override { return m_step; }

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
        m_dragBox->setParentBox(box());
        m_skinElements.setBox(box());
    }

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

    virtual void serialize(Serializer& s) const override;

protected:
    std::shared_ptr<IRelativeBox> m_box;
    std::shared_ptr<IRelativeBox> m_dragBox;
    Direction::Enum m_direction;

    bool m_alwaysShow;
    float m_step;
    std::shared_ptr<ButtonSkin> m_decButtonSkin;
    std::shared_ptr<IRelativeOffset> m_decButtonPosition;
    std::shared_ptr<ButtonSkin> m_incButtonSkin;
    std::shared_ptr<IRelativeOffset> m_incButtonPosition;
    std::shared_ptr<ButtonSkin> m_dragBarSkin;

    ObjectsCollection m_skinElements;
};

} }
