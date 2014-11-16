#pragma once

#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/math/Transform2.h>
#include <vector>

namespace gamebase {

class ObjectsCollection : public IMovable, public IDrawable {
public:
    ObjectsCollection(const std::shared_ptr<IObject>& mainObject = nullptr);

    void addChild(const std::shared_ptr<IObject>& object);

    virtual Transform2 position() const override;

    virtual Transform2 transform() const override;

    virtual void move(float time) override;

    virtual void loadResources() override;

    virtual void draw(const Transform2& globalPosition) const override;

private:
    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    IDrawable* m_mainDrawable;
    std::vector<IMovable*> m_movableObjects;
    std::vector<IDrawable*> m_drawableObjects;
};

}
