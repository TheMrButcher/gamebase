#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IFindable.h>
#include <gamebase/engine/IPositionable.h>
#include <gamebase/geom/IGeometry.h>

namespace gamebase {

class GAMEBASE_API FindableGeometry : public IFindable {
public:
    FindableGeometry(
        IPositionable* position,
        const std::shared_ptr<IGeometry>& geom);

    FindableGeometry(
        IObject* associatedObj,
        const IPositionable* position,
        const std::shared_ptr<IGeometry>& geom);

    virtual IObject* find(
        const Vec2& point, const Transform2& globalPosition) override;

private:
    IObject* m_associatedFindableObj;
    const IPositionable* m_findablePos;
    std::shared_ptr<IGeometry> m_findableGeom;
};

}
