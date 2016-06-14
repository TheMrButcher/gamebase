#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/math/Transform2.h>
#include <map>
#include <vector>

namespace gamebase { namespace impl {

class GAMEBASE_API ObjectsSelector : public Drawable, public Registrable,
    public IFindable, public ISerializable {
public:
    ObjectsSelector(const IPositionable* position = nullptr);

    virtual void insertObject(int id, const std::shared_ptr<IObject>& object);
    virtual void removeObject(int id);
    virtual void clear();

    virtual void select(int id);
    virtual bool isAnySelected() const { return m_objDescs.find(m_currentObjectID) != m_objDescs.end(); }
    virtual int selected() const { return m_currentObjectID; }
    
    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

protected:
    const IPositionable* m_position;
    std::map<int, std::shared_ptr<IObject>> m_objects;

    struct ObjectDesc {
        ObjectDesc()
            : drawable(nullptr)
            , positionable(nullptr)
            , findable(nullptr)
        {}

        IDrawable* drawable;
        IPositionable* positionable;
        IFindable* findable;
    };

    std::map<int, ObjectDesc> m_objDescs;
    int m_currentObjectID;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
};

class Deserializer;
GAMEBASE_API void deserializeObjectsSelectorElements(
    Deserializer& deserializer, ObjectsSelector* obj);

} }
