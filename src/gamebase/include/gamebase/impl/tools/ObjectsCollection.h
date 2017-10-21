/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/engine/IMovable.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/engine/ISelectable.h>
#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>
#include <gamebase/impl/serial/ISerializable.h>
#include <gamebase/math/Transform2.h>
#include <vector>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;

class GAMEBASE_API ObjectsCollection : public Drawable, public Registrable,
    public IPositionable, public IMovable, public IFindable, public ISerializable {
public:
    ObjectsCollection(IPositionable* position = nullptr);

    void addObject(const std::shared_ptr<IObject>& object);
    void replaceObject(int id, const std::shared_ptr<IObject>& object);
    bool removeObject(int id);
    int findObject(IObject* obj) const;

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
    ObjectDesc registerObject(const std::shared_ptr<IObject>& object);

    IPositionable* m_position;
    std::vector<std::shared_ptr<IObject>> m_objects;
    std::vector<ObjectDesc> m_objectDescs;
    ISelectable* m_associatedSelectable;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
};

} }
