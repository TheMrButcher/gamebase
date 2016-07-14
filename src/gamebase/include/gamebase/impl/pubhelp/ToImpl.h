/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>
#include <memory>

namespace gamebase {

class Layout;
class DrawObj;
class GameObj;
    
namespace impl {
    
template <typename T>
struct ToImpl {
    static IObject* castRaw(const T& obj)
    {
        return dynamic_cast<IObject*>(obj.getImpl().get());
    }

    static std::shared_ptr<IObject> castShared(const T& obj)
    {
        return std::dynamic_pointer_cast<IObject>(obj.getImpl().getShared());
    }

    static SmartPointer<IObject> castSmart(const T& obj)
    {
        return obj.getImpl();
    }
};

template <>
struct GAMEBASE_API ToImpl<Layout> {
    static IObject* castRaw(const Layout& obj);
    static std::shared_ptr<IObject> castShared(const Layout& obj);
    static SmartPointer<IObject> castSmart(const Layout& obj);
};

template <>
struct GAMEBASE_API ToImpl<DrawObj> {
    static IObject* castRaw(const DrawObj& obj);
    static std::shared_ptr<IObject> castShared(const DrawObj& obj);
    static SmartPointer<IObject> castSmart(const DrawObj& obj);
};

template <>
struct GAMEBASE_API ToImpl<GameObj> {
    static IObject* castRaw(const GameObj& obj);
    static std::shared_ptr<IObject> castShared(const GameObj& obj);
    static SmartPointer<IObject> castSmart(const GameObj& obj);
};

} }
