#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/math/Transform2.h>
#include <vector>

namespace gamebase {

class GAMEBASE_API ObjectsCollection : public IMovable, public IDrawable, public IFindable {
public:
    ObjectsCollection(const std::shared_ptr<IObject>& mainObject = nullptr);

    ObjectsCollection(IObject* mainObject);

    void addChild(const std::shared_ptr<IObject>& object);

    virtual Transform2 position() const override;

    virtual Transform2 transform() const override;

    virtual void setParentPosition(const IPositionable* parent) override;

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void move(float time) override;

    virtual void loadResources() override;

    virtual void draw(const Transform2& globalPosition) const override;

private:
    void setMainObject(IObject* mainObject);

    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    IDrawable* m_mainDrawable;
    IFindable* m_mainFindable;
    std::vector<IMovable*> m_movableObjects;
    std::vector<IDrawable*> m_drawableObjects;
    std::vector<IFindable*> m_findableObjects;
};

}
