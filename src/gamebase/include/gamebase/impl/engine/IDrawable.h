/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/engine/IObject.h>
#include <gamebase/impl/relbox/IRelativeBox.h>
#include <gamebase/math/Transform2.h>
#include <memory>

namespace gamebase { namespace impl {

class GAMEBASE_API IDrawable : public virtual IObject {
public:
    virtual void loadResources() = 0;

    virtual void draw(const Transform2& globalPosition) const { drawAt(globalPosition); }

    virtual void drawAt(const Transform2& position) const = 0;

    virtual void setBox(const BoundingBox& allowedBox) = 0;

    virtual BoundingBox box() const = 0;

    virtual void setVisible(bool visible) = 0;

    virtual bool isVisible() const = 0;

    float width() const { return box().width(); }

    float height() const { return box().height(); }
};

inline void loadIfNeededNoCheck(const std::shared_ptr<IRelativeBox>& box, IObject* obj)
{
    if (auto* drawable = dynamic_cast<IDrawable*>(obj)) {
        drawable->setBox(box->get());
        drawable->loadResources();
    }
}

inline void loadIfNeeded(const std::shared_ptr<IRelativeBox>& box, IObject* obj)
{
    if (box->isValid()) {
        loadIfNeededNoCheck(box, obj);
    }
}

} }
