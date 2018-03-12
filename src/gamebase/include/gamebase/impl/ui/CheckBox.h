/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/skin/base/CheckBoxSkin.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/tools/Handle.h>
#include <functional>

namespace gamebase { namespace impl {

class GAMEBASE_API CheckBox : public OffsettedPosition, public FindableGeometry,
    public Selectable, public Drawable, public Registrable, public ISerializable {
public:
    CheckBox(
        const std::shared_ptr<CheckBoxSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setCallback(const std::function<void(bool)>& callback) { m_callback = callback; }
    void setCallback(const std::function<void()>& callback);

    bool isChecked() const { return m_checked; }

    void setChecked(bool status);

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& globalPosition) const
    {
        m_skin->draw(globalPosition);
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
    std::function<void(bool)> m_callback;
    std::shared_ptr<CheckBoxSkin> m_skin;
    bool m_checked;
    Handle m_callbackHandle;
};

} }
