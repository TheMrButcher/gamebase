#pragma once

#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/FindableGeometry.h>
#include <gamebase/engine/Skin.h>
#include <functional>

namespace gamebase {

class GAMEBASE_API Button : public OffsettedPosition, public FindableGeometry, public Selectable, public IDrawable {
public:
    Button(
        const Vec2& position,
        const std::shared_ptr<IGeometry>& geom,
        const std::shared_ptr<Skin>& skin,
        const std::function<void()>& callback = nullptr);

    void setCallback(const std::function<void()>& callback) { m_callback = callback; }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void loadResources() override
    {
        m_skin->loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const
    {
        m_skin->draw(m_pos * globalPosition);
    }

private:
    std::function<void()> m_callback;
    std::shared_ptr<Skin> m_skin;
};

}
