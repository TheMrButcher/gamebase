/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/gameview/Database.h>
#include <gamebase/impl/gameview/IIndex.h>
#include <gamebase/impl/pos/OffsettedPosition.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/reg/Registrable.h>
#include <gamebase/impl/findable/IFindable.h>
#include <boost/optional.hpp>
#include <vector>
#include <memory>
#include <map>

namespace gamebase { namespace impl {

class GroupLayer;
class ILayerAdapter;

class GAMEBASE_API ILayer : public Registrable, public Drawable, public OffsettedPosition {
public:
    ILayer()
        : OffsettedPosition(nullptr)
        , Drawable(this)
        , m_id(-1)
    {}

    int id() const { return m_id; }
    void setID(int id) { m_id = id; }

    virtual void setViewBox(const BoundingBox& viewBox) = 0;
	virtual void setGameBox(const boost::optional<BoundingBox>& gameBox) = 0;
    virtual void setDependent() = 0;

    virtual bool hasObject(int id) const = 0;
    virtual bool hasObject(IObject* obj) const = 0;
    bool hasObject(const std::shared_ptr<IObject>& obj) const { return hasObject(obj.get()); }

    virtual int addObject(const std::shared_ptr<IObject>& obj) = 0;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) = 0;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) = 0;

    virtual void removeObject(int id) = 0;
    virtual void removeObject(IObject* obj) = 0;
    void removeObject(const std::shared_ptr<IObject>& obj) { removeObject(obj.get()); }

    virtual IObject* getIObject(int id) const = 0;
    template <typename ObjType>
    ObjType* getObject(int id) const
    {
        return dynamic_cast<ObjType*>(getIObject(id));
    }

    virtual void clear() = 0;
    
    template <typename ObjType>
    std::vector<ObjType*> getObjects() const
    {
        const auto& objs = objectsAsList();
        std::vector<ObjType*> result;
        result.reserve(objs.size());
        for (auto it = objs.begin(); it != objs.end(); ++it) {
            auto* obj = it->get();
            if (auto castedObj = dynamic_cast<ObjType*>(obj))
                result.push_back(castedObj);
        }
        return result;
    }

    template <typename DataType>
    DataType& data(int id)
    {
        auto idb = getDatabase();
        if (!idb) {
            setDatabase(std::unique_ptr<Database<DataType>>(new Database<DataType>()));
            idb = getDatabase();
        }
        if (!idb)
            THROW_EX() << "Layer of type " << typeid(*this).name() << " can't hold data";
        auto* db = static_cast<Database<DataType>*>(idb);
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

    std::vector<IObject*> findByBox(const BoundingBox& box) const
    {
        auto index = getIndex();
        if (!index)
            return getIObjects();
        auto drawables = index->drawablesByBox(box);
        std::vector<IObject*> result;
        result.reserve(drawables.size());
        for (auto it = drawables.begin(); it != drawables.end(); ++it)
            result.push_back(*it);
        return result;
    }

    std::vector<IObject*> getIObjects() const
    {
        const auto& objects = objectsAsList();
        std::vector<IObject*> result;
        result.reserve(objects.size());
        for (auto it = objects.begin(); it != objects.end(); ++it)
            result.push_back(it->get());
        return std::move(result);
    }

    template <typename ObjType>
    std::vector<ObjType*> all() const
    {
        return getObjects<ObjType>();
    }
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const = 0;
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const = 0;

    virtual size_t size() const = 0;

	virtual void update() = 0;

protected:
	void updateOffset(const BoundingBox& viewBox)
	{
		auto offset = viewBox.isValid() ? -viewBox.center() : Vec2(0, 0);
		setOffset(offset);
	}
    
private:
    friend class GroupLayer;
    friend class ILayerAdapter;

    virtual const IIndex* getIndex() const = 0;
    virtual IDatabase* getDatabase() const = 0;
    virtual void setDatabase(std::unique_ptr<IDatabase> db) = 0;
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const = 0;
    virtual const std::vector<Drawable*>& drawablesInView() const = 0;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const = 0;

    int m_id;
};

} }
