/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/gameview/IOrder.h>
#include <gamebase/impl/gameview/IIndex.h>
#include <gamebase/impl/gameview/Database.h>
#include <gamebase/impl/tools/Handle.h>
#include <unordered_map>

namespace gamebase { namespace impl {

class GAMEBASE_API ImmobileLayer : public ILayer, public IFindable, public ISerializable {
public:
    ImmobileLayer();
    ~ImmobileLayer();
    
    void setIndex(const std::shared_ptr<IIndex>& index);
    void setOrder(const std::shared_ptr<IOrder>& order);

    template <typename DataType>
    DataType& data(int id)
    {
        if (!m_db)
            m_db.reset(new Database<DataType>());
        auto* db = static_cast<Database<DataType>*>(m_db.get());
        return db->get(id);
    }

    template <typename DataType>
    DataType& data(IObject* obj)
    {
        return data<DataType>(indexByObj(obj));
    }

    template <typename DataType>
    DataType& data(const std::shared_ptr<IObject>& obj)
    {
        return data<DataType>(indexByObj(obj.get()));
    }

    template <typename ObjType>
    std::vector<ObjType*> findByBox(const BoundingBox& box) const
    {
        if (!m_index)
            return all<ObjType>();
        updateIndexIfNeeded();
        auto drawables = m_index->drawablesByBox(box);
        std::vector<ObjType*> result;
        result.reserve(drawables.size());
        for (auto it = drawables.begin(); it != drawables.end(); ++it) {
            auto* obj = *it;
            if (auto castedObj = dynamic_cast<ObjType*>(obj))
                result.push_back(castedObj);
        }
        return result;
    }

    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void setGameBox(const boost::optional<BoundingBox>& gameBox) override;
    virtual void setDependent() override { m_independent = false; }

    virtual bool hasObject(int id) const override { return m_objects.find(id) != m_objects.end(); }
    virtual bool hasObject(IObject* obj) const override { return m_indexByObj.find(obj) != m_indexByObj.end(); }

    virtual int addObject(const std::shared_ptr<IObject>& obj) override;
    void addObjects(const std::vector<std::shared_ptr<IObject>>& objects);
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override;

    virtual void removeObject(int id) override;
    virtual void removeObject(IObject* obj) override { removeObject(indexByObj(obj)); }

    virtual IObject* getIObject(int id) const override;

    virtual void clear() override;
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const override;
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const override { return getIObjectSPtr(indexByObj(obj)); }

    virtual size_t size() const override { return m_objects.size(); }
    virtual void update() override;

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override { return m_curBox; }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

protected:
    void setNeedUpdate() const
    {
        m_cachedDrawables.clear();
        m_needToUpdate = true;
    }

    void delayedUpdate() const;

protected:
    virtual const std::vector<Drawable*>& drawablesInView() const override;

private:
    virtual const IIndex* getIndex() const override { return m_index.get(); }
    virtual IDatabase* getDatabase() const override { return m_db.get(); }
    virtual void setDatabase(std::unique_ptr<IDatabase> db) override { m_db = std::move(db); }
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const override;

    int indexByObj(IObject* obj) const;
    void addToIndex(int id, IObject* obj);
    void resetCaches() const;
    virtual void updateIndexIfNeeded() const override;
    void calcDrawables() const;
    
    BoundingBox m_curBox;

    struct ObjDesc {
        std::shared_ptr<IObject> obj;
        Drawable* drawable;
        IFindable* findable;
    };

    std::unordered_map<int, ObjDesc> m_objects;
    std::unordered_map<IObject*, int> m_indexByObj;

    BoundingBox m_viewBox;
    boost::optional<BoundingBox> m_gameBox;
    bool m_isGameBoxInited;
    mutable bool m_needToUpdate;
    int m_nextID;
    std::shared_ptr<IIndex> m_index;
    std::shared_ptr<IOrder> m_order;
    mutable std::vector<Drawable*> m_cachedDrawables;
    mutable std::vector<std::shared_ptr<IObject>> m_cachedAllObjs;
    std::unique_ptr<PropertiesRegisterBuilder> m_registerBuilder;
    std::unique_ptr<IDatabase> m_db;
    bool m_independent;
    mutable bool m_isLocked;
    mutable Handle m_updateHandle;
};

} }
