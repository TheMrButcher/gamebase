#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <memory>

namespace gamebase {

class Layout;
    
namespace impl {
    
template <typename T>
struct ToImpl {
    static std::shared_ptr<IObject> cast(const T& obj)
    {
        return std::dynamic_pointer_cast<IObject>(t.getImpl());
    }
};

template <>
struct GAMEBASE_API ToImpl<Layout> {
    static std::shared_ptr<IObject> cast(const Layout& obj);
};

} }
