#pragma once

#include <gamebase/impl/pubhelp/Helpers.h>

#define GAMEBASE_DEFINE_GAME_OBJ_ANIMATION_PIMPL() \
public: \
    Animation(GameObj* obj) : m_obj(obj) {} \
private: \
    GameObj* m_obj

#define GAMEBASE_DEFINE_GAME_OBJ_PIMPL() \
public: \
    typedef impl::GameObjAdapter Impl; \
    GameObj(const std::shared_ptr<Impl>& impl = nullptr) : anim(this), m_impl(impl) {} \
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; } \
private: \
    std::shared_ptr<Impl> m_impl
