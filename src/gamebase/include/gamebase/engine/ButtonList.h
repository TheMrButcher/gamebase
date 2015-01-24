#pragma once

#include <gamebase/engine/ButtonListSkin.h>

namespace gamebase {

class GAMEBASE_API ButtonList : public OffsettedPosition, public IFindable, public IDrawable {
public:
    ButtonList(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<ButtonListSkin>& skin);

    void addButton(const std::shared_ptr<Button>& button);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;
    virtual void loadResources() override;
    virtual void draw(const Transform2& globalPosition) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

private:
    std::shared_ptr<ButtonListSkin> m_skin;
    class ScrollPosition;
    std::shared_ptr<ScrollPosition> m_scrollOffset;
    ObjectsCollection m_list;
    std::shared_ptr<ScrollBar> m_scroll;
    BoundingBox m_box;
};

}
