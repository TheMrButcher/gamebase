#pragma once

#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/math/Transform2.h>
#include <map>
#include <vector>

namespace gamebase {

class ObjectsSelector : public IMovable, public IDrawable {
public:
    ObjectsSelector(const std::shared_ptr<IObject>& mainObject = nullptr);

    void addChild(int id, const std::shared_ptr<IObject>& object);

    void select(int id);

    virtual Transform2 position() const override;

    virtual Transform2 transform() const override;

    virtual void move(float time) override;

    virtual void loadResources() override;

    virtual void draw(const Transform2& globalPosition) const override;

private:
    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    std::map<int, IMovable*> m_movableObjects;
    std::map<int, IDrawable*> m_drawableObjects;
    int m_currentObjectID;
};

}
