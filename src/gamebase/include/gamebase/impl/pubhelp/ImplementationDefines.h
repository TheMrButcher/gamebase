/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

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
    inline ClassName::operator bool() const { return static_cast<bool>(m_impl); }

#define GAMEBASE_DEFINE_OFFSETTED_POS_METHODS(ClassName) \
    inline Vec2 ClassName::pos() const { return m_impl->getOffset(); } \
    inline void ClassName::setPos(float x, float y) { m_impl->setOffset(Vec2(x, y)); } \
    inline void ClassName::setPos(const Vec2& v) { m_impl->setOffset(v); } \
    inline BoundingBox ClassName::box() const { return m_impl->transformedBox(); } \
    inline BoundingBox ClassName::movedBox() const { return m_impl->movedBox(); } \
    inline float ClassName::width() const { return box().width(); } \
    inline float ClassName::height() const { return box().height(); }

#define GAMEBASE_DEFINE_SELECTABLE_METHODS(ClassName) \
    inline bool ClassName::isEnabled() const { return m_impl->selectionState() != impl::SelectionState::Disabled; } \
    inline void ClassName::setEnabled(bool value) { if (value) enable(); else disable(); } \
    inline void ClassName::enable() { m_impl->setSelectionState(impl::SelectionState::None); } \
    inline void ClassName::disable() { m_impl->setSelectionState(impl::SelectionState::Disabled); }

#define GAMEBASE_DEFINE_COLOR_METHODS(ClassName) \
    inline Color ClassName::color() const { return m_impl->color(); } \
    inline void ClassName::setColor(float r, float g, float b, float a) { m_impl->setColor(Color(r, g, b, a)); } \
    inline void ClassName::setColor(const Color& color) { m_impl->setColor(color); }

#define GAMEBASE_DEFINE_TEXTURE_METHODS(ClassName) \
    inline const std::string& ClassName::imageName() const { return m_impl->imageName(); } \
    inline void ClassName::setImageName(const std::string& name) { m_impl->setImageName(name); } \
    inline void ClassName::setSizes(float width, float height) { m_impl->setFixedBox(width, height); }

#define GAMEBASE_DEFINE_TEXT_METHODS(ClassName) \
    inline const std::string& ClassName::text() const { return m_impl->text(); } \
    inline void ClassName::setText(const std::string& text) { m_impl->setText(text); }

#define GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ClassName) \
    GAMEBASE_DEFINE_DRAWABLE_METHODS(ClassName); \
    GAMEBASE_DEFINE_OFFSETTED_POS_METHODS(ClassName);

#define GAMEBASE_DEFINE_UI_ACTIVE_ELEMENT_METHODS(ClassName) \
    GAMEBASE_DEFINE_UI_PASSIVE_ELEMENT_METHODS(ClassName); \
    GAMEBASE_DEFINE_SELECTABLE_METHODS(ClassName);
