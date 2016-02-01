#pragma once

#include <gamebase/engine/ILayer.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/IRelativeBox.h>

namespace gamebase {

class GAMEBASE_API GameView : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    GameView(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    Vec2 viewCenter() const { return m_viewBox.center(); }
    const BoundingBox& viewBox() const { return m_viewBox; }
    Vec2 setViewCenter(const Vec2& v);

    const BoundingBox& gameBox() const { return m_gameBox; }
    void setGameBox(const BoundingBox& box);

    bool isMouseOn() const;
    Vec2 mouseCoords() const;

    int addLayer(const std::shared_ptr<ILayer>& layer) { return m_canvas->addObject(layer); }
    void insertLayer(int id, const std::shared_ptr<ILayer>& layer) { m_canvas->insertObject(id, layer); }
    void insertLayers(const std::map<int, std::shared_ptr<ILayer>>& layers);
    void removeLayer(int id) { m_canvas->removeObject(id); }
    void removeLayer(ILayer* layer) { m_canvas->removeObject(layer); }
    void removeLayer(const std::shared_ptr<ILayer>& layer) { removeLayer(layer.get()); }

    template <typename LayerType>
    LayerType* getLayer(int id) const
    {
        return dynamic_cast<LayerType*>(m_canvas->getIObject(id));
    }

    void clear() { m_canvas->clear(); }

    virtual bool isSelectableByPoint(const Vec2& point) const override { return false; }
    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2& point) const override;
    virtual void loadResources() override { m_canvas->loadResources(); }
    virtual void drawAt(const Transform2& position) const override;
    virtual void setBox(const BoundingBox& allowedBox) override;

    virtual BoundingBox box() const override
    {
        return m_box->get();
    }

    virtual void registerObject(PropertiesRegisterBuilder* builder) override;
    
    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IRelativeBox> m_box;
    BoundingBox m_viewBox;
    BoundingBox m_gameBox;
    std::shared_ptr<CanvasLayout> m_canvas;
};

}
