#pragma once

#include <gamebase/engine/ILayer.h>
#include <gamebase/engine/IOrder.h>
#include <gamebase/engine/CanvasLayout.h>
#include <unordered_map>

namespace gamebase {

class GameView;

class GAMEBASE_API GroupLayer : public ILayer, public IFindable, public ISerializable {
public:
    GroupLayer();

    void setOrder(const std::shared_ptr<IOrder>& order);
    int addLayer(const std::shared_ptr<ILayer>& layer);

    void insertLayer(int id, const std::shared_ptr<ILayer>& layer)
    {
        registerLayer(id, layer.get());
        m_canvas->insertObject(id, layer);
    }

    void insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers);

    void removeLayer(int id)
    {
        m_canvas->removeObject(id);
        removeFromRegister(id);
    }

    void removeLayer(ILayer* layer) { removeLayer(layer->id()); }

    void removeLayer(const std::shared_ptr<ILayer>& layer) { removeLayer(layer.get()); }

    template <typename LayerType>
    LayerType* getLayer(int id) const
    {
        auto it = m_layers.find(id);
        if (it == m_layers.end())
            THROW_EX() << "Can't find layer with ID: " << id;
        auto result = dynamic_cast<LayerType*>(it->second);
        if (!result)
            THROW_EX() << "Type of layer with ID=" << id << " differs from required";
        return result;
    }

    virtual void setViewBox(const BoundingBox& viewBox);
    virtual void setGameBox(const BoundingBox& gameBox);
    virtual void setDependent() { THROW_EX() << "setDependent: Not supported"; }

    virtual bool hasObject(int id) const override { THROW_EX() << "hasObject: Not supported"; }
    virtual bool hasObject(IObject* obj) const override { THROW_EX() << "hasObject: Not supported"; }
    virtual int addObject(const std::shared_ptr<IObject>& obj) { THROW_EX() << "addObject: Not supported"; }
    virtual void insertObject(int id, const std::shared_ptr<IObject>& obj) { THROW_EX() << "insertObject: Not supported"; }
    virtual void insertObjects(const std::map<int, std::shared_ptr<IObject>>& objects) { THROW_EX() << "insertObjects: Not supported"; }
    virtual void removeObject(int id) { THROW_EX() << "removeObject: Not supported"; }
    virtual void removeObject(IObject* obj) { THROW_EX() << "removeObject: Not supported"; }
    virtual IObject* getIObject(int id) const { THROW_EX() << "getIObject: Not supported"; }
    virtual void clear() { m_canvas->clear(); }
    
    virtual std::shared_ptr<IObject> getIObjectSPtr(int id) const { THROW_EX() << "getIObjectSPtr: Not supported"; }
    virtual std::shared_ptr<IObject> getIObjectSPtr(IObject* obj) const { THROW_EX() << "getIObjectSPtr: Not supported"; }

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override;
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
    void registerLayer(int id, ILayer* layer);
    void removeFromRegister(int id);

    virtual const std::vector<std::shared_ptr<IObject>>& objectsAsList() const { THROW_EX() << "objectsAsList: Not supported"; }
    virtual const std::vector<Drawable*>& drawablesInView() const { THROW_EX() << "drawablesInView: Not supported"; }
    virtual const std::vector<IFindable*>& findablesByBox(const BoundingBox& box) const { THROW_EX() << "findablesByBox: Not supported"; }

    BoundingBox m_viewBox;
    Transform2 m_layersShift;
    std::shared_ptr<CanvasLayout> m_canvas;
    std::unordered_map<int, ILayer*> m_layers;
    std::shared_ptr<IOrder> m_order;
    GameView* m_gameView;
    mutable std::vector<Drawable*> m_cachedDrawables;
    mutable std::vector<IFindable*> m_cachedFindables;
    bool m_inited;
};

}
