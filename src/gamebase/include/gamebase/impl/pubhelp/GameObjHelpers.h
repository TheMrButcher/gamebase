/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/pubhelp/Helpers.h>
#include <gamebase/impl/adapt/GameObjAdapter.h>

#define GAMEBASE_DEFINE_GAME_OBJ_ANIMATION_PIMPL() \
public: \
    Animation(GameObj* obj) : m_obj(obj) {} \
private: \
    GameObj* m_obj

#define GAMEBASE_DEFINE_GAME_OBJ_PIMPL() \
public: \
    typedef impl::GameObjAdapter Impl; \
    GameObj(const std::shared_ptr<Impl>& impl = nullptr) : anim(this), m_impl(impl) {} \
    GameObj(const GameObj& other) : anim(this), m_impl(other.m_impl) {} \
    GameObj& operator=(const GameObj& other) { m_impl = other.m_impl; return *this; } \
    const std::shared_ptr<Impl>& getImpl() const { return m_impl; } \
private: \
    std::shared_ptr<Impl> m_impl
