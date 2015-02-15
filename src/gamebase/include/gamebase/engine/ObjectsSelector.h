#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/math/Transform2.h>
#include <map>
#include <vector>

namespace gamebase {

class GAMEBASE_API ObjectsSelector : public Drawable, public IMovable, public IFindable {
public:
    ObjectsSelector(const std::shared_ptr<IObject>& mainObject = nullptr);

    void addChild(int id, const std::shared_ptr<IObject>& object);

    void select(int id);

    virtual Transform2 position() const override;
    virtual void setParentPosition(const IPositionable* parent) override;

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void move(float time) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

private:
    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    std::map<int, IMovable*> m_movableObjects;
    std::map<int, IDrawable*> m_drawableObjects;
    std::map<int, IFindable*> m_findableObjects;
    int m_currentObjectID;
};

}
