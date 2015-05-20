#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/CheckBoxSkin.h>
#include <gamebase/serial/ISerializable.h>
#include <functional>

namespace gamebase {

class GAMEBASE_API CheckBox : public OffsettedPosition, public FindableGeometry,
    public Selectable, public Drawable, public Registrable, public ISerializable {
public:
    CheckBox(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<CheckBoxSkin>& skin);

    void setCallback(const std::function<void(bool)>& callback) { m_callback = callback; }

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
        m_offset->setBoxes(allowedBox, box());
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
};

}
