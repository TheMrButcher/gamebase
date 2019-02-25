/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/geom/Box.h>

#define GAMEBASE_DEFINE_PIMPL(ClassName, ImplClassName) \
public: \
    typedef impl::ImplClassName Impl; \
    ClassName(const impl::SmartPointer<Impl>& impl = nullptr) : m_impl(impl) {} \
    const impl::SmartPointer<Impl>& getImpl() const { return m_impl; } \
private: \
    impl::SmartPointer<Impl> m_impl

#define GAMEBASE_DEFINE_PIMPL_STD_SP(ClassName, ImplClassName) \
public: \
    typedef impl::ImplClassName Impl; \
    ClassName(const std::shared_ptr<Impl>& impl = nullptr) : m_impl(impl) {} \
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; } \
private: \
    std::shared_ptr<Impl> m_impl

#define GAMEBASE_DEFINE_DRAWABLE_METHODS(ClassName) \
    inline bool ClassName::isVisible() const { return m_impl->isVisible(); } \
    inline void ClassName::setVisible(bool value) { m_impl->setVisible(value); } \
    inline void ClassName::show() { m_impl->setVisible(true); } \
    inline void ClassName::hide() { m_impl->setVisible(false); } \
    inline bool ClassName::isMouseOn() const { return impl::isMouseOn(m_impl.get()); } \
    inline ClassName::operator bool() const { return static_cast<bool>(m_impl); }

#define GAMEBASE_DEFINE_OFFSETTED_POS_METHODS(ClassName) \
    inline Vec2 ClassName::pos() const { return m_impl->getOffset(); } \
    inline void ClassName::setPos(float x, float y) { m_impl->setOffset(Vec2(x, y)); } \
    inline void ClassName::setPos(const Vec2& v) { m_impl->setOffset(v); } \
    inline Box ClassName::box() const { return m_impl->transformedBox(); } \
    inline float ClassName::width() const { return m_impl->box().width(); } \
    inline float ClassName::height() const { return m_impl->box().height(); }

#define GAMEBASE_DEFINE_SELECTABLE_METHODS(ClassName) \
    inline bool ClassName::isEnabled() const { return m_impl->selectionState() != impl::SelectionState::Disabled; } \
    inline void ClassName::setEnabled(bool value) { if (value) enable(); else disable(); } \
    inline void ClassName::enable() { m_impl->setSelectionState(impl::SelectionState::None); } \
    inline void ClassName::disable() { m_impl->setSelectionState(impl::SelectionState::Disabled); }

#define GAMEBASE_DEFINE_COLOR_METHODS(ClassName) \
    inline Color ClassName::color() const { return m_impl->color().intColor(); } \
    inline void ClassName::setColor(int r, int g, int b, int a) { m_impl->setColor(impl::makeGLColor(r, g, b, a)); } \
    inline void ClassName::setColor(const Color& color) { m_impl->setColor(impl::makeGLColor(color)); }

#define GAMEBASE_DEFINE_TEXTURE_METHODS(ClassName) \
    inline const std::string& ClassName::imageName() const { return m_impl->imageName(); } \
    inline void ClassName::setImageName(const std::string& name) { m_impl->setImageName(name); } \
    inline void ClassName::setSize(float width, float height) { m_impl->setFixedBox(width, height); } \
    inline void ClassName::setSize(const Vec2& size) { m_impl->setFixedBox(size.x, size.y); }

#define GAMEBASE_DEFINE_TEXT_METHODS(ClassName) \
    inline const std::string& ClassName::text() const { return m_impl->text(); } \
    inline void ClassName::setText(const std::string& text) { m_impl->setText(text); }

#define GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ClassName) \
    GAMEBASE_DEFINE_DRAWABLE_METHODS(ClassName); \
    GAMEBASE_DEFINE_OFFSETTED_POS_METHODS(ClassName);

#define GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(ClassName) \
    GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ClassName); \
    GAMEBASE_DEFINE_SELECTABLE_METHODS(ClassName);
