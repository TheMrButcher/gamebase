#pragma once

#include <gamebase/math/Transform2.h>

namespace gamebase { namespace impl {

class PointGeometry;
class RectGeometry;

class IGeometry {
public:
    virtual ~IGeometry() {}

    virtual bool intersects(const IGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const = 0;

    virtual bool intersects(const PointGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const = 0;

    virtual bool intersects(const RectGeometry* other,
        const Transform2& thisTrans, const Transform2& otherTrans) const = 0;
};

} }
