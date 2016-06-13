#include <stdafx.h>
#include <gamebase/impl/pubhelp/FromImpl.h>
#include <gamebase/impl/adapt/LinearLayoutAdapter.h>
#include <gamebase/impl/adapt/CanvasLayoutAdapter.h>
#include <gamebase/ui/Layout.h>

namespace gamebase { namespace impl {

std::shared_ptr<ILayoutAdapter> FromImpl<Layout>::cast(
    const SmartPointer<IObject>& obj)
{
    auto canvasLayout = SmartPointer<CanvasLayout>(obj);
    if (canvasLayout)
        return std::make_shared<CanvasLayoutAdapter>(canvasLayout);
    auto linearLayout = SmartPointer<LinearLayout>(obj);
    if (linearLayout)
        return std::make_shared<LinearLayoutAdapter>(linearLayout);
    return nullptr;
}

} }
