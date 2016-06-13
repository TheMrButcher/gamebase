#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <memory>

namespace gamebase {

class Layout;
    
namespace impl {

class ILayoutAdapter;
    
template <typename T>
struct FromImpl {
    static std::shared_ptr<typename T::Impl> cast(const std::shared_ptr<IObject>& obj)
    {
        return std::dynamic_pointer_cast<typename T::Impl>(obj);
    }
};

template <>
struct GAMEBASE_API FromImpl<Layout> {
    static std::shared_ptr<ILayoutAdapter> cast(const std::shared_ptr<IObject>& obj);
};

} }
