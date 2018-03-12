/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/gameview/IOrder.h>
#include <gamebase/impl/gameview/IIndex.h>

namespace gamebase { namespace impl {

class GAMEBASE_API StaticLayer : public ILayer, public IFindable, public ISerializable {
public:
    StaticLayer();
    
    void setIndex(const std::shared_ptr<IIndex>& index);
    void setOrder(const std::shared_ptr<IOrder>& order);

    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void setGameBox(const boost::optional<BoundingBox>& gameBox) override;
    virtual void setDependent() override { m_independent = false; }
    
    virtual bool hasObject(int id) const override { return m_canvas->hasObject(id); }
    virtual bool hasObject(IObject* obj) const override { return m_canvas->hasObject(obj); }

    virtual int addObject(const std::shared_ptr<IObject>& obj) override;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override;

    virtual void removeObject(int id) override { THROW_EX() << "Not supported"; }
    virtual void removeObject(IObject* obj) override { THROW_EX() << "Not supported"; }

    virtual IObject* getIObject(int id) const override { return m_canvas->getIObject(id); }

    virtual void clear() override;
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const override { return m_canvas->getIObjectSPtr(id); }
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const override;

    virtual size_t size() const override { return m_canvas->objectsAsList().size(); }
    virtual void update() override;

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
    virtual const IIndex* getIndex() const override { return m_index.get(); }
    virtual IDatabase* getDatabase() const override { return nullptr; }
    virtual void setDatabase(std::unique_ptr<IDatabase> db) override { THROW_EX() << "setDatabase: Not supported"; }
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override { return m_canvas->objectsAsList(); }
    virtual const std::vector<Drawable*>& drawablesInView() const override;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const override;
    virtual void updateIndexIfNeeded() const;

    void addToIndex(int id, IObject* obj);
    void calcDrawables() const;

    std::shared_ptr<CanvasLayout> m_canvas;
    BoundingBox m_viewBox;
    boost::optional<BoundingBox> m_gameBox;
    bool m_isGameBoxInited;
    mutable bool m_needToUpdate;
    std::shared_ptr<IIndex> m_index;
    std::shared_ptr<IOrder> m_order;
    mutable std::vector<Drawable*> m_cachedDrawables;
    std::vector<std::pair<int, Drawable*>> m_objsToIndex;
    bool m_independent;
};

} }
