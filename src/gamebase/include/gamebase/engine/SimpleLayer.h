#pragma once

#include <gamebase/engine/ILayer.h>
#include <gamebase/engine/CanvasLayout.h>

namespace gamebase {

class GAMEBASE_API SimpleLayer : public ILayer, public Registrable, public IFindable, public ISerializable {
public:
    SimpleLayer();

    virtual void setViewBox(const BoundingBox& viewBox) override;

    virtual int addObject(const std::shared_ptr<IObject>& obj) override { return m_canvas->addObject(obj); }
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) override { m_canvas->insertObject(id, obj); }
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) override { m_canvas->insertObjects(objects); }
    virtual void removeObject(int id) override { m_canvas->removeObject(id); }
    virtual void removeObject(IObject* obj) override { m_canvas->removeObject(obj); }
    virtual IObject* getIObject(int id) const override { return m_canvas->getIObject(id); }

    virtual void clear() override { m_canvas->clear(); }
    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const override { return m_canvas->objectsAsList(); }

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
    std::shared_ptr<CanvasLayout> m_canvas;
};

}
