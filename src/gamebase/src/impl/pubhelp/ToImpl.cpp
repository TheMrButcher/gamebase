#include <stdafx.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <gamebase/ui/Layout.h>
#include <gamebase/drawobj/DrawObj.h>
#include <gamebase/gameobj/GameObj.h>

namespace gamebase { namespace impl {
    
IObject* ToImpl<Layout>::castRaw(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<Layout>::castShared(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

IObject* ToImpl<DrawObj>::castRaw(const DrawObj& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<DrawObj>::castShared(const DrawObj& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

IObject* ToImpl<GameObj>::castRaw(const GameObj& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<GameObj>::castShared(const GameObj& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

} }
