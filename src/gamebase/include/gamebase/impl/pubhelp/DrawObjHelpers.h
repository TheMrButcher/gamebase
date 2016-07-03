#pragma once

#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/impl/adapt/IDrawObjAdapter.h>

#define GAMEBASE_DEFINE_DRAW_OBJ_PIMPL() \
public: \
    typedef impl::IDrawObjAdapter Impl; \
    DrawObj(const std::shared_ptr<Impl>& impl = nullptr) : m_impl(impl) {} \
    DrawObj(const DrawObj& other) : m_impl(other.m_impl) {} \
    template <typename T> DrawObj(const T& other) { *this = other; } \
    DrawObj& operator=(const DrawObj& other) { m_impl = other.m_impl; return *this; } \
    template <typename T> DrawObj& operator=(const T& other) { *this = impl::wrap<DrawObj>(impl::unwrapSmart(other)); return *this; } \
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; } \
private: \
    std::shared_ptr<Impl> m_impl
