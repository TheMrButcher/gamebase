/**
 * Copyright (c) 2018 Slavnejshev Filipp
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
    void setView(float x, float y);
    void setView(const Vec2& v);
    Box viewBox() const;
    
    bool isMouseOn() const;
    Vec2 mousePos() const;

    template <typename DataType> void remove(const Layer<DataType>& layer);
    template <typename DataType> Layer<DataType> get(int id) const;
    template <typename DataType> Layer<DataType> get(const std::string& name) const;
    template <typename T> T child(const std::string& name) const;

    Layer<void> get(int id) const;
    Layer<void> get(const std::string& name) const;
    void remove(int id);
    void remove(const std::string& name);
    void clear();

    bool isVisible() const;
    void setVisible(bool value);
    void show();
    void hide();

    Box box() const;
    float width() const;
    float height() const;

    operator bool() const;

    template <typename DataType> Layer<DataType> load(const std::string& fileName);
    template <typename DataType> Layer<DataType> load(int id, const std::string& fileName);
    Layer<void> load(const std::string& fileName);
    Layer<void> load(int id, const std::string& fileName);

    GAMEBASE_DEFINE_PIMPL(GameView, GameView);
};

/////////////// IMPLEMENTATION ///////////////////

inline Vec2 GameView::view() const { return m_impl->viewCenter(); }
inline void GameView::setView(float x, float y) { m_impl->setViewCenter(Vec2(x, y)); }
inline void GameView::setView(const Vec2& v) { m_impl->setViewCenter(v); }
inline Box GameView::viewBox() const { return m_impl->viewBox(); }
inline bool GameView::isMouseOn() const { return m_impl->isMouseOn(); }
inline Vec2 GameView::mousePos() const { return m_impl->mouseCoords(); }
template <typename DataType> inline void GameView::remove(const Layer<DataType>& layer) { m_impl->removeObject(layer.getImpl().get()); }
template <typename DataType> inline Layer<DataType> GameView::get(int id) const { return Layer<DataType>(impl::SmartPointer<impl::ILayer>(m_impl->getLayer<impl::ILayer>(id))); }
template <typename DataType> inline Layer<DataType> GameView::get(const std::string& name) const { return Layer<DataType>(impl::SmartPointer<impl::ILayer>(m_impl->getLayer(name))); }
template <typename T> inline T GameView::child(const std::string& name) const { return impl::findAndWrap<T>(m_impl.get(), name); }
inline Layer<void> GameView::get(int id) const { return get<void>(id); }
inline Layer<void> GameView::get(const std::string& name) const { return get<void>(name); }
inline void GameView::remove(int id) { m_impl->removeLayer(id); }
inline void GameView::remove(const std::string& name) { m_impl->removeLayer(name); }
inline void GameView::clear() { m_impl->clear(); }
inline Box GameView::box() const { return m_impl->viewBox(); }
inline float GameView::width() const { return m_impl->box().width(); }
inline float GameView::height() const { return m_impl->box().height(); }
template <typename DataType> inline Layer<DataType> GameView::load(const std::string& fileName)
{
    auto layerImpl = impl::deserializeLayer(fileName);
    m_impl->addLayer(layerImpl);
    return Layer<DataType>(impl::SmartPointer<impl::ILayer>(layerImpl.get()));
}
template <typename DataType> inline Layer<DataType> GameView::load(int id, const std::string& fileName)
{
    auto layerImpl = impl::deserializeLayer(fileName);
    m_impl->insertLayer(id, layerImpl);
    return Layer<DataType>(impl::SmartPointer<impl::ILayer>(layerImpl.get()));
}
inline Layer<void> GameView::load(const std::string& fileName) { return load<void>(fileName); }
inline Layer<void> GameView::load(int id, const std::string& fileName) { return load<void>(id, fileName); }
GAMEBASE_DEFINE_DRAWABLE_METHODS(GameView);

}
