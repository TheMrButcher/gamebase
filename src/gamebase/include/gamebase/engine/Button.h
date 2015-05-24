#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/ButtonSkin.h>
#include <gamebase/serial/ISerializable.h>
#include <functional>

namespace gamebase {

class GAMEBASE_API Button : public OffsettedPosition, public FindableGeometry,
    public Selectable, public Drawable, public Registrable, public ISerializable {
public:
    Button(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ButtonSkin>& skin,
        const std::function<void()>& callback = nullptr);

    void setCallback(const std::function<void()>& callback) { m_callback = callback; }

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
    
    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

protected:
    std::function<void()> m_callback;
    std::shared_ptr<ButtonSkin> m_skin;
};

}
