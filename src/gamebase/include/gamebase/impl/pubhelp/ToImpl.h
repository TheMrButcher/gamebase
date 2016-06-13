#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <memory>

namespace gamebase {

class Layout;
    
namespace impl {
    
template <typename T>
struct ToImpl {
    static IObject* castRaw(const T& obj)
    {
        return dynamic_cast<IObject*>(t.getImpl().get());
    }

    static std::shared_ptr<IObject> castShared(const T& obj)
    {
        return std::dynamic_pointer_cast<IObject>(t.getImpl().getShared());
    }
};

template <>
struct GAMEBASE_API ToImpl<Layout> {
    static IObject* castRaw(const Layout& obj);
    static std::shared_ptr<IObject> castShared(const Layout& obj);
};

} }
