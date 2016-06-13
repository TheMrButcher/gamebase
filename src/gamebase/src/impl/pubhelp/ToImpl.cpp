#include <stdafx.h>
#include <gamebase/impl/pubhelp/ToImpl.h>
#include <gamebase/ui/Layout.h>

namespace gamebase { namespace impl {

std::shared_ptr<IObject> ToImpl<Layout>::cast(const Layout& obj)
{
    return obj.getImpl()->getInternalObj();
}

} }
