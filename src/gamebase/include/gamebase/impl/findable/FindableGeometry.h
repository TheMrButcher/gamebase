/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/findable/IFindable.h>
#include <gamebase/impl/pos/IPositionable.h>
#include <gamebase/impl/engine/IDrawable.h>
#include <gamebase/impl/geom/IRelativeGeometry.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FindableGeometry : public virtual IFindable {
public:
    FindableGeometry(
        const IObject* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    std::shared_ptr<IRelativeGeometry> geometry() const { return m_findableGeom; }

    virtual bool isSelectableByPoint(const Vec2& point) const override;

    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2&) const override { return nullptr; }

protected:
    const IObject* m_findableObj;
    std::shared_ptr<IRelativeGeometry> m_findableGeom;
    mutable bool m_findableGeometryInited;
    mutable const IPositionable* m_findablePos;
    mutable const IDrawable* m_findableDrawable;
};

} }
