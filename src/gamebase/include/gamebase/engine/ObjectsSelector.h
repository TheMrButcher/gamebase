#pragma once

#include <gamebase/GameBaseAPI.h>
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
    public IFindable, public ISerializable {
public:
    ObjectsSelector(const IPositionable* position = nullptr);

    virtual void addObject(int id, const std::shared_ptr<IObject>& object);

    virtual void select(int id);
    
    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    const IPositionable* m_position;
    std::map<int, std::shared_ptr<IObject>> m_objects;
    std::map<int, IDrawable*> m_drawableObjects;
    std::map<int, IFindable*> m_findableObjects;
    int m_currentObjectID;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
};

class Deserializer;
GAMEBASE_API void deserializeObjectsSelectorElements(
    Deserializer& deserializer, ObjectsSelector* obj);

}
