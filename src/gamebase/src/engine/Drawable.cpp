#include <stdafx.h>
#include <gamebase/engine/Drawable.h>
#include <gamebase/engine/Application.h>

namespace gamebase {

bool isMouseOn(Drawable* drawable)
{
    auto fullPos = drawable->drawPosition()->fullTransform();
    auto fullBox = drawable->box().transformed(fullPos);
    return fullBox.contains(app->input().mousePosition());
}

}
