#pragma once

#include <gamebase/engine/PanelSkin.h>
#include <gamebase/engine/ObjectsCollection.h>

namespace gamebase {

class GAMEBASE_API Panel : public OffsettedPosition, public Drawable, public IFindable, public Selectable {
public:
    Panel(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<PanelSkin>& skin);

    void addObject(const std::shared_ptr<IObject>& obj)
    {
        m_objects.addChild(obj);
    }

    virtual void setSelectionState(SelectionState::Enum state) override {}

    void setAssociatedSelectable(ISelectable* selectable) override
    {
        Selectable::setAssociatedSelectable(selectable);
        m_objects.setAssociatedSelectable(selectable);
    }

    void setCloseCallback(const std::function<void()>& closeCallback)
    {
        m_closeCallback = closeCallback;
    }
    
    void resetPosition();
    void close();

    virtual Transform2 position() const override;
    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

private:
    std::shared_ptr<PanelSkin> m_skin;
    class DragOffset;
    std::shared_ptr<DragOffset> m_dragOffset;
    ObjectsCollection m_objects;
    std::function<void()> m_closeCallback;
};

}
