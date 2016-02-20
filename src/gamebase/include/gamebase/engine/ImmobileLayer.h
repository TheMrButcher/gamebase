#pragma once

#include <gamebase/engine/ILayer.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/IOrder.h>
#include <gamebase/engine/IIndex.h>
#include <gamebase/engine/Database.h>
#include <unordered_map>

namespace gamebase {

class GAMEBASE_API ImmobileLayer : public ILayer, public IFindable, public ISerializable {
public:
    ImmobileLayer();
    
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

    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void setGameBox(const BoundingBox& gameBox) override;
    virtual void setDependent() override { m_independent = false; }

    virtual int addObject(const std::shared_ptr<IObject>& obj) override;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override;

    virtual void removeObject(int id) override;
    virtual void removeObject(IObject* obj) override { removeObject(indexByObj(obj)); }

    virtual IObject* getIObject(int id) const override;

    virtual void clear() override;
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const override;
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const override { return getIObjectSPtr(indexByObj(obj)); }

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;
    virtual BoundingBox box() const override { return m_curBox; }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override;
    virtual const std::vector<Drawable*>& drawablesInView() const override;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const override;

    int indexByObj(IObject* obj) const;
    void addToIndex(int id, IObject* obj);
    void resetCaches() const;
    void updateIndexIfNeeded() const;
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
};

}
