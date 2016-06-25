#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>

namespace gamebase {

class Layout;
class DrawObj;
class GameObj;
    
namespace impl {

class ILayoutAdapter;
class IDrawObjAdapter;
class GameObjAdapter;
    
template <typename T>
struct FromImpl {
    static SmartPointer<typename T::Impl> cast(const SmartPointer<IObject>& obj)
    {
        return SmartPointer<typename T::Impl>(obj);
    }
};

template <>
struct GAMEBASE_API FromImpl<Layout> {
    static std::shared_ptr<ILayoutAdapter> cast(const SmartPointer<IObject>& obj);
};

template <>
struct GAMEBASE_API FromImpl<DrawObj> {
    static std::shared_ptr<IDrawObjAdapter> cast(const SmartPointer<IObject>& obj);
};

template <>
struct GAMEBASE_API FromImpl<GameObj> {
    static std::shared_ptr<GameObjAdapter> cast(const SmartPointer<IObject>& obj);
};

} }
