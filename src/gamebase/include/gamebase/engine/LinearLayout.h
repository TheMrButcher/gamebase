#pragma once

#include <gamebase/engine/LinearLayoutSkin.h>
#include <gamebase/engine/ObjectsCollection.h>
#include <gamebase/engine/OffsettedPosition.h>

namespace gamebase {

class GAMEBASE_API LinearLayout : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    LinearLayout(
        const std::shared_ptr<LinearLayoutSkin>& skin,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    void addObject(const std::shared_ptr<IObject>& obj);
    const std::vector<std::shared_ptr<IObject>>& objects() const { return m_list.objects(); }
    void clear() { m_list.clear(); }
    Direction::Enum direction() const { return m_skin->direction(); }

    void setFixedBox(float width, float height);
    void update();

    void setAssociatedSelectable(ISelectable* selectable);

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
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

