#include <stdafx.h>
#include <gamebase/engine/ObjectReflection.h>
#include <gamebase/engine/Application.h>

namespace gamebase {

std::shared_ptr<IObject> ObjectReflection::findChildByPoint(const Vec2&) const
{
    if (!isVisible())
        return nullptr;
    return m_children.findChildByPoint(
        position().inversed() * app->input().mousePosition());
}

}
