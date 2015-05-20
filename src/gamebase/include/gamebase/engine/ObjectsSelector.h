#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/math/Transform2.h>
#include <map>
#include <vector>

namespace gamebase {

class GAMEBASE_API ObjectsSelector : public Drawable, public Registrable,
    public IMovable, public IFindable, public ISerializable {
public:
    ObjectsSelector(const std::shared_ptr<IObject>& mainObject = nullptr);

    void addObject(int id, const std::shared_ptr<IObject>& object);

    void select(int id);

    virtual Transform2 position() const override;
    virtual void setParentPosition(const IPositionable* parent) override;

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void move() override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IObject> m_mainObj;
    std::map<int, std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    std::map<int, IMovable*> m_movableObjects;
    std::map<int, IDrawable*> m_drawableObjects;
    std::map<int, IFindable*> m_findableObjects;
    int m_currentObjectID;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
};

}
