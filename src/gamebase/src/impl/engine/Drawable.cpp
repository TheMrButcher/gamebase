#include <stdafx.h>
#include <gamebase/impl/engine/Drawable.h>
#include <gamebase/impl/app/Application.h>

namespace gamebase { namespace impl {

bool isMouseOn(Drawable* drawable)
{
    auto fullPos = drawable->drawPosition()->fullTransform();
    auto fullBox = drawable->box().transformed(fullPos);
    return fullBox.contains(app->input().mousePosition());
}

} }
