#pragma once

#include <gamebase/engine/ScrollDragBarSkin.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/IInputProcessor.h>
#include <gamebase/engine/FloatValue.h>
#include <gamebase/engine/FixedOffset.h>
#include <boost/optional.hpp>

namespace gamebase {

class GAMEBASE_API ScrollDragBar : public OffsettedPosition, public Selectable, public FindableGeometry, public IInputProcessor, public IDrawable {
public:
    ScrollDragBar(
        const std::shared_ptr<FixedOffset>& position,
        const std::shared_ptr<ScrollDragBarSkin>& skin,
        const std::shared_ptr<FloatValue>& controlledValue);

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        m_skin->draw(transform() * globalPosition);
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

private:
    std::shared_ptr<ScrollDragBarSkin> m_skin;
    std::shared_ptr<FloatValue> m_controlledValue;
    boost::optional<float> m_mousePos;
};

}
