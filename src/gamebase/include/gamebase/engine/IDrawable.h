#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IObject.h>
#include <gamebase/engine/IRelativeBox.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

class GAMEBASE_API IDrawable : public virtual IObject {
public:
    virtual void loadResources() = 0;

    virtual void draw(const Transform2& globalPosition) const { drawAt(globalPosition); }

    virtual void drawAt(const Transform2& position) const = 0;

    virtual void setBox(const BoundingBox& allowedBox) = 0;

    virtual BoundingBox box() const = 0;

    virtual void setVisible(bool visible) = 0;

    virtual bool isVisible() const = 0;
};

inline void loadIfNeeded(const std::shared_ptr<IRelativeBox>& box, IObject* obj)
{
    if (box->isValid()) {
        if (auto* drawable = dynamic_cast<IDrawable*>(obj)) {
            drawable->setBox(box->get());
            drawable->loadResources();
        }
    }
}

}
