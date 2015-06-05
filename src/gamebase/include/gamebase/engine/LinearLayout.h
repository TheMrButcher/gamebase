#pragma once

#include <gamebase/engine/LinearLayoutSkin.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/OffsettedPosition.h>

namespace gamebase {

class GAMEBASE_API LinearLayout : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    LinearLayout(
        const std::shared_ptr<IRelativeOffset>& position,
        const std::shared_ptr<LinearLayoutSkin>& skin);

    void addObject(const std::shared_ptr<IObject>& obj);
    const std::vector<std::shared_ptr<IObject>>& objects() const { return m_list.objects(); }
    Direction::Enum direction() const { return m_skin->direction(); }

    void setAssociatedSelectable(ISelectable* selectable);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_skin->box();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<LinearLayoutSkin> m_skin;
    ObjectsCollection m_list;
    BoundingBox m_allowedBox;
};

}

