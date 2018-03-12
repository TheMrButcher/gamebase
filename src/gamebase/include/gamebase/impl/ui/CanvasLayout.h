/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/ObjectsCollection.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/impl/relbox/IResizable.h>
#include <gamebase/impl/engine/Adjustment.h>
#include <boost/optional.hpp>

namespace gamebase { namespace impl {

class GAMEBASE_API CanvasLayout : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable, public IResizable {
public:
    CanvasLayout(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    bool hasObject(int id) const;
    bool hasObject(IObject* obj) const;
    int addObject(const std::shared_ptr<IObject>& obj);
    void addObjects(const std::vector<std::shared_ptr<IObject>>& objects);
    void insertObject(int id, const std::shared_ptr<IObject>& obj);
    void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects);
    void removeObject(int id);
    void removeObject(IObject* obj);
    void removeObject(const std::shared_ptr<IObject>& obj) { removeObject(obj.get()); }
    void removeObjects(const std::vector<int>& ids);
    void removeObjects(const std::vector<IObject*>& objs);

    IObject* getIObject(int id) const
    {
        auto it = m_objects.find(id);
        if (it == m_objects.end())
            THROW_EX() << "Can't find object with ID: " << id;
        return it->second.get();
    }

    template <typename ObjType>
    ObjType* getObject(int id) const
    {
        return dynamic_cast<ObjType*>(getIObject(id));
    }

    std::shared_ptr<IObject> getIObjectSPtr(int id) const
    {
        auto it = m_objects.find(id);
        if (it == m_objects.end())
            THROW_EX() << "Can't find object with ID: " << id;
        return it->second;
    }

    std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const;

    int nextID() const { return m_nextID; }

    Adjustment::Enum adjustment() const { return m_adjustment; }
    void setAdjustment(Adjustment::Enum value) { m_adjustment = value; }

    virtual void setFixedBox(float width, float height) override;

    void update();
    void clear();

    ObjectsCollection& objectsCollection() { return m_list; }
    const ObjectsCollection& objectsCollection() const { return m_list; }
    const std::vector<std::shared_ptr<IObject>>& objectsAsList() const { return m_list.objects(); }
    const std::map<int, std::shared_ptr<IObject>>& objectsAsMap() const { return m_objects; }

    void setAssociatedSelectable(ISelectable* selectable);

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_curBox;
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void refill();
    void updateBox();

    std::shared_ptr<IRelativeBox> m_box;
    boost::optional<BoundingBox> m_parentBox;
    BoundingBox m_curBox;
    std::map<int, std::shared_ptr<IObject>> m_objects;
    ObjectsCollection m_list;
    int m_nextID;
    Adjustment::Enum m_adjustment;
};

} }
