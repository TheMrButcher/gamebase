#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/ISelectable.h>
#include <gamebase/math/Transform2.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API ObjectsCollection : public Drawable, public IMovable, public IFindable {
public:
    ObjectsCollection(const std::shared_ptr<IObject>& mainObject = nullptr);

    ObjectsCollection(IObject* mainObject);

    void addChild(const std::shared_ptr<IObject>& object);

    virtual Transform2 position() const override;
    virtual void setParentPosition(const IPositionable* parent) override;

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void move(float time) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    std::vector<std::shared_ptr<IObject>>::iterator begin() { return m_objects.begin(); }
    std::vector<std::shared_ptr<IObject>>::iterator end() { return m_objects.end(); }
    size_t size() const { return m_objects.size(); }
    bool empty() const { return m_objects.empty(); }

    void setAssociatedSelectable(ISelectable* selectable);

private:
    void setMainObject(IObject* mainObject);

    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    IDrawable* m_mainDrawable;
    IFindable* m_mainFindable;
    std::vector<IMovable*> m_movableObjects;
    std::vector<IDrawable*> m_drawableObjects;
    std::vector<IFindable*> m_findableObjects;
    ISelectable* m_associatedSelectable;
};

}
