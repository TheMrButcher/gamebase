/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/geom/Segment.h>

namespace gamebase { namespace impl {

namespace {
boost::optional<Vec2> vertAndInclinedSegmentsIntersection(
    const Segment& vertSegment, const Segment& segment)
{
    float y = segment.substitute(vertSegment.a.x);
    std::pair<float, float> minMaxY = std::minmax(vertSegment.a.y, vertSegment.b.y);
    if (minMaxY.first <= y && y <= minMaxY.second)
        return Vec2(vertSegment.a.x, y);
    return boost::none;
}

const float EPSILON = 0.001f;
}

boost::optional<Vec2> intersection(const Segment& s1, const Segment& s2)
{
    BoundingBox box1 = s1.boundingBox();
    BoundingBox box2 = s2.boundingBox();
    BoundingBox boxIntersection = intersect(box1, box2);
    if (!boxIntersection.isValid())
        return boost::none;

    if (s1.isVertical() || s2.isVertical()) {
        if (s1.isVertical() && s2.isVertical())
            return boxIntersection.bottomLeft;
        return s1.isVertical()
            ? vertAndInclinedSegmentsIntersection(s1, s2)
            : vertAndInclinedSegmentsIntersection(s2, s1);
    }

    float kSub = s2.k() - s1.k();
    if (kSub == 0)
        return boost::none;
    float x = (s1.c() - s2.c()) / kSub;
    float y = s1.substitute(x);
    Vec2 result(x, y);
    if (extend(boxIntersection, EPSILON).contains(result))
        return result;
    return boost::none;
}

} }
