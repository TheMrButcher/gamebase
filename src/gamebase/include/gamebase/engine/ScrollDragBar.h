#pragma once

#include <gamebase/engine/ScrollDragBarSkin.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/IInputProcessor.h>
#include <gamebase/engine/FloatValue.h>
#include <gamebase/engine/FixedOffset.h>
#include <boost/optional.hpp>

namespace gamebase {

class GAMEBASE_API ScrollDragBar : public OffsettedPosition, public Selectable,
    public FindableGeometry, public IInputProcessor, public Drawable, public Registrable {
public:
    ScrollDragBar(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ScrollDragBarSkin>& skin);

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
        m_offset->setBoxes(allowedBox, m_skin->box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void setSelectionState(SelectionState::Enum state) override;
    virtual void processInput(const InputRegister& input) override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;

private:
    std::shared_ptr<ScrollDragBarSkin> m_skin;
    std::shared_ptr<FloatValue> m_controlledHorizontal;
    std::shared_ptr<FloatValue> m_controlledVertical;
    boost::optional<Vec2> m_mousePos;
};

}