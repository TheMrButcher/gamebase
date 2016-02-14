#pragma once

#include <gamebase/engine/ILayer.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/IOrder.h>
#include <gamebase/engine/IIndex.h>

namespace gamebase {

class GAMEBASE_API StaticLayer : public ILayer, public IFindable, public ISerializable {
public:
    StaticLayer();
    
    void setIndex(const std::shared_ptr<IIndex>& index);
    void setOrder(const std::shared_ptr<IOrder>& order);

    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void setGameBox(const BoundingBox& gameBox) override;

    virtual int addObject(const std::shared_ptr<IObject>& obj) override;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override;

    virtual void removeObject(int id) override { THROW_EX() << "Not supported"; }
    virtual void removeObject(IObject* obj) override { THROW_EX() << "Not supported"; }

    virtual IObject* getIObject(int id) const override { return m_canvas->getIObject(id); }

    virtual void clear() override;
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override { return m_canvas->objectsAsList(); }
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const override { return m_canvas->getIObjectSPtr(id); }
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const override;

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override { return nullptr; }
    virtual void loadResources() override { m_canvas->loadResources(); }
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_canvas->setBox(allowedBox);
        setPositionBoxes(allowedBox, allowedBox);
    }

    virtual BoundingBox box() const override { return m_canvas->box(); }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    void addToIndex(int id, IObject* obj);

    std::shared_ptr<CanvasLayout> m_canvas;
    BoundingBox m_viewBox;
    bool m_isGameBoxInited;
    std::shared_ptr<IIndex> m_index;
    std::shared_ptr<IOrder> m_order;
    mutable std::vector<Drawable*> m_cachedDrawables;
    std::vector<std::pair<int, Drawable*>> m_objsToIndex;
};

}
