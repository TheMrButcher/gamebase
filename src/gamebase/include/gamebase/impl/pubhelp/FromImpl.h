#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/pubhelp/SmartPointer.h>

namespace gamebase {

class Layout;
    
namespace impl {

class ILayoutAdapter;
    
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

} }
