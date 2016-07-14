/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/ui/CanvasLayout.h>

namespace gamebase { namespace impl {

class GAMEBASE_API SimpleLayer : public ILayer, public IFindable, public ISerializable {
public:
    SimpleLayer();

    virtual void setViewBox(const BoundingBox& viewBox) override;
    virtual void setGameBox(const BoundingBox& gameBox) override {}
    virtual void setDependent() override {}

    virtual bool hasObject(int id) const override { return m_canvas->hasObject(id); }
    virtual bool hasObject(IObject* obj) const override { return m_canvas->hasObject(obj); }

    virtual int addObject(const std::shared_ptr<IObject>& obj) override;
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override;
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override;

    virtual void removeObject(int id) override { m_canvas->removeObject(id); }
    virtual void removeObject(IObject* obj) override { m_canvas->removeObject(obj); }

    virtual IObject* getIObject(int id) const override { return m_canvas->getIObject(id); }

    virtual void clear() override { m_canvas->clear(); }
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const override { return m_canvas->getIObjectSPtr(id); }
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const override { return m_canvas->getIObjectSPtr(obj); }

    virtual size_t size() const override { return m_canvas->objectsAsList().size(); }

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override { m_canvas->loadResources(); }
    virtual void drawAt(const Transform2& position) const override { m_canvas->draw(position); }
    virtual void setBox(const BoundingBox& allowedBox) override
    {
        m_canvas->setBox(allowedBox);
        setPositionBoxes(allowedBox, allowedBox);
    }

    virtual BoundingBox box() const override { return m_canvas->box(); }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    virtual const IIndex* getIndex() const override { return nullptr; }
    virtual IDatabase* getDatabase() const override { return nullptr; }
    virtual void setDatabase(std::unique_ptr<IDatabase> db) override { THROW_EX() << "setDatabase: Not supported"; }
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override { return m_canvas->objectsAsList(); }
    virtual const std::vector<Drawable*>& drawablesInView() const override;
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const override;

    std::shared_ptr<CanvasLayout> m_canvas;
};

} }
