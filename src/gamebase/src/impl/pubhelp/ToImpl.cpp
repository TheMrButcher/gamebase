#include <stdafx.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <gamebase/ui/Layout.h>

namespace gamebase { namespace impl {
    
IObject* ToImpl<Layout>::castRaw(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().get();
}

std::shared_ptr<IObject> ToImpl<Layout>::castShared(const Layout& obj)
{
    return obj.getImpl()->getInternalObj().getShared();
}

} }
