#pragma once

#include <gamebase/engine/RadioButtonGroup.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/CheckBoxSkin.h>
#include <functional>

namespace gamebase {

class GAMEBASE_API RadioButton : public OffsettedPosition, public FindableGeometry, public Selectable, public Drawable {
public:
    RadioButton(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<CheckBoxSkin>& skin,
        const std::shared_ptr<RadioButtonGroup>& group);

    void setGroup(const std::shared_ptr<RadioButtonGroup>& group);

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
        m_offset->setBoxes(allowedBox, box());
    }

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

private:
    friend class RadioButtonGroup;

    void setCheckedImpl(bool status);

    std::shared_ptr<CheckBoxSkin> m_skin;
    std::shared_ptr<RadioButtonGroup> m_group;
    bool m_checked;
    size_t m_index;
};

}
