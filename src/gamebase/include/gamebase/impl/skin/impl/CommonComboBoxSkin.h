/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ComboBoxSkin.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/serial/ISerializable.h>

namespace gamebase { namespace impl {

class GAMEBASE_API CommonComboBoxSkin : public ComboBoxSkin, public ISerializable {
public:
    CommonComboBoxSkin(const std::shared_ptr<IRelativeBox>& box);

    void addElement(const std::shared_ptr<IObject>& element)
    {
        m_skinElements.addObject(element);
    }

    void setTextBoxDisabled(bool value) { m_textBoxDisabled = value; }
    bool textBoxDisabled() const { return m_textBoxDisabled; }

    void setOpenButtonSkin(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setTextBoxSkin(
        const std::shared_ptr<TextBoxSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setButtonListSkin(
        const std::shared_ptr<ButtonListSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    virtual std::shared_ptr<ToggleButton> createOpenButton() const override;
    virtual std::shared_ptr<TextBox> createTextBox() const override;
    virtual std::shared_ptr<ButtonList> createList() const override;

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

    bool m_textBoxDisabled;

    std::shared_ptr<ButtonSkin> m_openButtonSkin;
    std::shared_ptr<IRelativeOffset> m_openButtonPosition;
    std::shared_ptr<TextBoxSkin> m_textBoxSkin;
    std::shared_ptr<IRelativeOffset> m_textBoxPosition;
    std::shared_ptr<ButtonListSkin> m_buttonListSkin;
    std::shared_ptr<IRelativeOffset> m_buttonListPosition;

    ObjectsCollection m_skinElements;
};

} }
