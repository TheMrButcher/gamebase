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
        IObject* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    FindableGeometry(
        IObject* associatedObj,
        const IObject* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

private:
    IObject* m_associatedFindableObj;
    const IObject* m_findableObj;
    std::shared_ptr<IRelativeGeometry> m_findableGeom;
    bool m_inited;
    const IPositionable* m_findablePos;
    const IDrawable* m_findableDrawable;
};

}
