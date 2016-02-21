#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/OffsettedPosition.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Registrable.h>
#include <gamebase/engine/IFindable.h>
#include <vector>
#include <memory>
#include <map>

namespace gamebase {

class GroupLayer;

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
    virtual void setGameBox(const BoundingBox& gameBox) = 0;
    virtual void setDependent() = 0;

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
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const = 0;
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const = 0;
    
private:
    friend class GroupLayer;

    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const = 0;
    virtual const std::vector<Drawable*>& drawablesInView() const = 0;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const = 0;

    int m_id;
};

}
