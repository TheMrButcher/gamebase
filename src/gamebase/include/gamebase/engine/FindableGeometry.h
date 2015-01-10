#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/geom/IRelativeGeometry.h>

namespace gamebase {

class GAMEBASE_API FindableGeometry : public IFindable {
public:
    FindableGeometry(
        IPositionable* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    FindableGeometry(
        IObject* associatedObj,
        const IPositionable* position,
        const std::shared_ptr<IRelativeGeometry>& geom);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

private:
    IObject* m_associatedFindableObj;
    const IPositionable* m_findablePos;
    std::shared_ptr<IRelativeGeometry> m_findableGeom;
};

}
