/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/IGameBox.h>
#include <gamebase/impl/gameview/ILayer.h>
#include <gamebase/impl/ui/CanvasLayout.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <unordered_map>

namespace gamebase { namespace impl {

class GAMEBASE_API GameView : public OffsettedPosition, public Drawable,
    public Registrable, public IFindable, public ISerializable {
public:
    GameView(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeOffset>& position = nullptr);

    Vec2 viewCenter() const { return m_viewBox.center(); }
    const BoundingBox& viewBox() const { return m_viewBox; }
    Vec2 setViewCenter(const Vec2& v);

    BoundingBox gameBox() const;
    void setGameBox(const BoundingBox& box);
    void setGameBox(const std::shared_ptr<IRelativeBox>& box);
	void setGameBox(const std::shared_ptr<IGameBox>& gameBox);

    bool isMouseOn() const;
    Vec2 mouseCoords() const;

    int addLayer(const std::shared_ptr<ILayer>& layer)
    {
        auto id = m_nextID++;
        insertLayer(id, layer);
        return id;
    }

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

    void clear() { m_canvas->clear(); }

    int nextID() const { return m_nextID; }

    void registerLayer(int id, ILayer* layer)
    {
        m_nextID = std::max(m_nextID, id + 1);
        m_layers[id] = layer;
        layer->setID(id);
    }

    void removeFromRegister(int id)
    {
        m_layers.erase(id);
    }

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
	void initGameBox();

	BoundingBox m_parentBox;
    std::shared_ptr<IRelativeBox> m_box;
    BoundingBox m_viewBox;
	bool m_isLimited;
	std::shared_ptr<IGameBox> m_gameBox;
    std::shared_ptr<CanvasLayout> m_canvas;
    std::unordered_map<int, ILayer*> m_layers;
    int m_nextID;
};

} }
