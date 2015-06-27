#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/engine/IDrawable.h>
#include <gamebase/geom/IRelativeGeometry.h>

namespace gamebase {

class GAMEBASE_API FindableGeometry : public IFindable {
public:
    FindableGeometry(
        const IObject* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    virtual bool isSelectableByPoint(const Vec2& point) const override;

    virtual std::shared_ptr<IObject> findChildByPoint(const Vec2&) const override { return nullptr; }

private:
    const IObject* m_findableObj;
    std::shared_ptr<IRelativeGeometry> m_findableGeom;
    mutable bool m_findableGeometryInited;
    mutable const IPositionable* m_findablePos;
    mutable const IDrawable* m_findableDrawable;
};

}
