/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/ButtonSkin.h>
#include <gamebase/impl/ui/ToolTip.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/impl/engine/IInputProcessor.h>
#include <gamebase/tools/Delayed.h>
#include <functional>

namespace gamebase { namespace impl {

class GAMEBASE_API Button : public OffsettedPosition, public FindableGeometry,
    public Selectable, public Drawable, public IInputProcessor, public Registrable, public ISerializable {
public:
    Button(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setCallback(const std::function<void()>& callback) { m_callback = callback; }
    void setToolTip(const std::shared_ptr<ToolTip>& toolTip);

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void processInput(const InputRegister& input) override;
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

protected:
    std::function<void()> m_callback;
    std::shared_ptr<ButtonSkin> m_skin;
    std::shared_ptr<ToolTip> m_toolTip;
    Handle m_callbackHandle;
};

} }
