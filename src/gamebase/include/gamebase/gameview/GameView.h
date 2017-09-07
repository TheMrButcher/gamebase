/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/GameView.h>
#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/gameview/Layer.h>
#include <gamebase/gameview/LayerVoidData.h>

namespace gamebase {

class GameView {
public:
    Vec2 view() const;
    void setView(const Vec2& v);
    BoundingBox viewBox() const;

    BoundingBox gameBox() const;
    void setGameBox(const BoundingBox& box);
    
    bool isMouseOn() const;
    Vec2 mousePos() const;

    template <typename DataType> int add(const Layer<DataType>& layer);
    template <typename DataType> void insert(int id, const Layer<DataType>& layer);
    template <typename DataType> void remove(const Layer<DataType>& layer);
    template <typename DataType> Layer<DataType> get(int id) const;
    template <typename T> T child(const std::string& name) const;

    void remove(int id);
    void clear();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Vec2 pos() const;
    void setPos(float x, float y);
    void setPos(const Vec2& v);
    BoundingBox box() const;
    BoundingBox movedBox() const;
    float width() const;
    float height() const;

    operator bool() const;

    GAMEBASE_DEFINE_PIMPL(GameView, GameView);
};

/////////////// IMPLEMENTATION ///////////////////

inline Vec2 GameView::view() const { return m_impl->viewCenter(); }
inline void GameView::setView(const Vec2& v) { m_impl->setViewCenter(v); }
inline BoundingBox GameView::viewBox() const { return m_impl->viewBox(); }
inline BoundingBox GameView::gameBox() const { return m_impl->gameBox(); }
inline void GameView::setGameBox(const BoundingBox& box) { m_impl->setGameBox(box); }
inline bool GameView::isMouseOn() const { return m_impl->isMouseOn(); }
inline Vec2 GameView::mousePos() const { return m_impl->mouseCoords(); }
template <typename DataType> inline int GameView::add(const Layer<DataType>& layer) { return m_impl->addLayer(layer.getImpl().getShared()); }
template <typename DataType> inline void GameView::insert(int id, const Layer<DataType>& layer) { m_impl->insertObject(id, layer.getImpl().getShared()); }
template <typename DataType> inline void GameView::remove(const Layer<DataType>& layer) { m_impl->removeObject(layer.getImpl().get()); }
template <typename DataType> inline Layer<DataType> GameView::get(int id) const { return Layer(impl::SmartPointer<impl::ILayer>(m_impl->getLayer<impl::ILayer>(id))); }
template <typename T> inline T GameView::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline void GameView::remove(int id) { m_impl->removeLayer(id); }
inline void GameView::clear() { m_impl->clear(); }
GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(GameView);

}
