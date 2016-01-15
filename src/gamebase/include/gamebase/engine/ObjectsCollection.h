#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/IMovable.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/ISelectable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>
#include <gamebase/serial/ISerializable.h>
#include <gamebase/math/Transform2.h>
#include <vector>

namespace gamebase {

class PropertiesRegisterBuilder;

class GAMEBASE_API ObjectsCollection : public Drawable, public Registrable,
    public IPositionable, public IMovable, public IFindable, public ISerializable {
public:
    ObjectsCollection(IPositionable* position = nullptr);

    void addObject(const std::shared_ptr<IObject>& object);

    virtual Transform2 position() const override;
    virtual void setParentPosition(const IPositionable* parent) override;

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;

    virtual void move(float time) override;

    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override;

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

    struct ObjectDesc {
        ObjectDesc()
            : movable(nullptr)
            , drawable(nullptr)
            , positionable(nullptr)
            , findable(nullptr)
        {}

        IMovable* movable;
        IDrawable* drawable;
        IPositionable* positionable;
        IFindable* findable;
    };

    std::vector<std::shared_ptr<IObject>>::iterator begin() { return m_objects.begin(); }
    std::vector<std::shared_ptr<IObject>>::iterator end() { return m_objects.end(); }
    std::vector<std::shared_ptr<IObject>>::const_iterator begin() const { return m_objects.begin(); }
    std::vector<std::shared_ptr<IObject>>::const_iterator end() const { return m_objects.end(); }
    size_t size() const { return m_objects.size(); }
    bool empty() const { return m_objects.empty(); }
    void clear();

    const std::vector<std::shared_ptr<IObject>>& objects() const { return m_objects; }
    const std::vector<ObjectDesc>& objectDescs() const { return m_objectDescs; }

    void setAssociatedSelectable(ISelectable* selectable);

private:
    IPositionable* m_position;
    std::vector<std::shared_ptr<IObject>> m_objects;
    std::vector<ObjectDesc> m_objectDescs;
    ISelectable* m_associatedSelectable;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
};

}
