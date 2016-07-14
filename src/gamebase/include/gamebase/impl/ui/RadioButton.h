/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/ui/RadioButtonGroup.h>
#include <gamebase/impl/skin/base/CheckBoxSkin.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <functional>

namespace gamebase { namespace impl {

class GAMEBASE_API RadioButton : public OffsettedPosition, public FindableGeometry,
    public Selectable, public Drawable, public Registrable, public ISerializable {
public:
    RadioButton(
        const std::shared_ptr<CheckBoxSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    ~RadioButton();

    void setGroup(const std::shared_ptr<RadioButtonGroup>& group);

    void setIndexInGroup(int index) { m_index = index; }
    int indexInGroup() const { return m_index; }

    bool isChecked() const { return m_checked; }

    void setChecked();

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& position) const
    {
        m_skin->draw(position);
    }

    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        setPositionBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    friend class RadioButtonGroup;

    void setCheckedImpl(bool status);

    std::shared_ptr<CheckBoxSkin> m_skin;
    std::shared_ptr<RadioButtonGroup> m_group;
    bool m_checked;
    int m_index;
};

} }
