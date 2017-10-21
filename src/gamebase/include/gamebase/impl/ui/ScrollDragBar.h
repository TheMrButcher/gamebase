/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/skin/base/ButtonSkin.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/findable/FindableGeometry.h>
#include <gamebase/impl/engine/IInputProcessor.h>
#include <gamebase/impl/reg/FloatValue.h>
#include <gamebase/impl/relpos/FixedOffset.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <boost/optional.hpp>

namespace gamebase { namespace impl {

class GAMEBASE_API ScrollDragBar : public OffsettedPosition, public Selectable,
    public FindableGeometry, public IInputProcessor, public Drawable, public Registrable, public ISerializable {
public:
    ScrollDragBar(
        const std::shared_ptr<ButtonSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void setControlledHorizontal(const std::shared_ptr<FloatValue>& controlledValue)
    {
        m_controlledHorizontal = controlledValue;
    }

    void setControlledVertical(const std::shared_ptr<FloatValue>& controlledValue)
    {
        m_controlledVertical = controlledValue;
    }

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void drawAt(const Transform2& position) const override
    {
        m_skin->draw(position);
    }
    
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_skin->setBox(allowedBox);
        setPositionBoxes(allowedBox, m_skin->box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void setSelectionState(SelectionState::Enum state) override;
    virtual void processInput(const InputRegister& input) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<ButtonSkin> m_skin;
    std::shared_ptr<FloatValue> m_controlledHorizontal;
    std::shared_ptr<FloatValue> m_controlledVertical;
    boost::optional<Vec2> m_mousePos;
};

} }
