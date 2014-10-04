#include <stdafx.h>
#include <gamebase/geom/PolylineMesh.h>
#include <gamebase/geom/Segment.h>

namespace gamebase {

namespace {
short addVertex(
    std::vector<float>& vertices,
    const Vec2& point,
    const Vec2& extVec,
    float distance)
{
    short index = static_cast<short>(vertices.size() / 5);
    vertices.push_back(point.x);
    vertices.push_back(point.y);
    vertices.push_back(distance);
    vertices.push_back(extVec.x);
    vertices.push_back(extVec.y);
    return index;
}

void addTriangleIndices(std::vector<short>& indices,
    short i1, short i2, short i3)
{
    indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
}

void addQuadIndices(std::vector<short>& indices,
    short i1, short i2, short i3, short i4)
{
    addTriangleIndices(indices, i1, i2, i3);
    addTriangleIndices(indices, i2, i3, i4);
}

std::pair<Vec2, boost::optional<Vec2>> createCap(
    const Vec2& v0, const Vec2& v1, const Vec2& v2,
    const Vec2& n0, const Vec2& n1)
{
    Vec2 prev = v0 + n0;
    Vec2 cur1 = v1 + n0;
    Vec2 cur2 = v1 + n1;
    Vec2 next = v2 + n1;
    Segment segPrev(prev, cur1);
    Segment segNext(cur2, next);
    boost::optional<Vec2> intrs = intersection(
        segPrev, segNext);
    if (intrs)
        return std::make_pair(*intrs - v1, boost::none);
    return std::make_pair(n0, n1);
}
}

PolylineMesh buildPolylineMesh(
    const Vec2* points, size_t size, float width)
{
    if (width < 0)
        THROW_EX() << "Wrong polyline width: " << width;
    PolylineMesh result;
    if (size <= 1)
        return result;
    if (size > 8192)
        THROW_EX() << "Polyline is too long: " << size;

    float halfWidth = std::max(0.0f, width * 0.5f - 1.0f);
    float extHalfWidth = halfWidth + 1.5f;
    short leftId, midLeftId, midRightId, rightId;
    Vec2 normal;
    {
        normal = rotate90((points[1] - points[0]).normalize());
        leftId = addVertex(result.vertices, points[0], normal * extHalfWidth, 0);
        midLeftId = addVertex(result.vertices, points[0], normal * halfWidth, 1);
        midRightId = addVertex(result.vertices, points[0], -normal * halfWidth, 1);
        rightId = addVertex(result.vertices, points[0], -normal * extHalfWidth, 0);
    }

    size_t lastId = size - 1;
    for (size_t i = 1; i < lastId; ++i) {
        Vec2 newNormal = rotate90((points[i + 1] - points[i]).normalize());
        
        std::pair<Vec2, boost::optional<Vec2>> extCapLeft = createCap(
            points[i - 1], points[i], points[i + 1],
            normal * extHalfWidth, newNormal * extHalfWidth);
        std::pair<Vec2, boost::optional<Vec2>> capLeft = createCap(
            points[i - 1], points[i], points[i + 1],
            normal * halfWidth, newNormal * halfWidth);

        std::pair<Vec2, boost::optional<Vec2>> capRight = createCap(
            points[i - 1], points[i], points[i + 1],
            -normal * halfWidth, -newNormal * halfWidth);
        std::pair<Vec2, boost::optional<Vec2>> extCapRight = createCap(
            points[i - 1], points[i], points[i + 1],
            -normal * extHalfWidth, -newNormal * extHalfWidth);

        short curLeftId = addVertex(result.vertices, points[i], extCapLeft.first, 0);
        short curMidLeftId = addVertex(result.vertices, points[i], capLeft.first, 1);
        short curMidRightId = addVertex(result.vertices, points[i], capRight.first, 1);
        short curRightId = addVertex(result.vertices, points[i], extCapRight.first, 0);

        addQuadIndices(result.indices, leftId, midLeftId, curLeftId, curMidLeftId);
        addQuadIndices(result.indices, midLeftId, midRightId, curMidLeftId, curMidRightId);
        addQuadIndices(result.indices, midRightId, rightId, curMidRightId, curRightId);

        if ((capLeft.second || capRight.second) && !(capLeft.second && capRight.second)) {
            if (capLeft.second) {
                short curLeftId2 = addVertex(result.vertices, points[i], *extCapLeft.second, 0);
                short curMidLeftId2 = addVertex(result.vertices, points[i], *capLeft.second, 1);
                addTriangleIndices(result.indices, curMidLeftId, curMidLeftId2, curMidRightId);
                addQuadIndices(result.indices, curMidLeftId, curMidLeftId2, curLeftId, curLeftId2);
                curLeftId = curLeftId2;
                curMidLeftId = curMidLeftId2;
            } else {
                short curMidRightId2 = addVertex(result.vertices, points[i], *capRight.second, 1);
                short curRightId2 = addVertex(result.vertices, points[i], *extCapRight.second, 0);
                addTriangleIndices(result.indices, curMidRightId, curMidRightId2, curMidLeftId);
                addQuadIndices(result.indices, curMidRightId, curMidRightId2, curRightId, curRightId2);
                curMidRightId = curMidRightId2;
                curRightId = curRightId2;
            }
        }

        normal = newNormal;
        leftId = curLeftId;
        midLeftId = curMidLeftId;
        midRightId = curMidRightId;
        rightId = curRightId;
    }

    short curLeftId = addVertex(result.vertices, points[lastId], normal * extHalfWidth, 0);
    short curMidLeftId = addVertex(result.vertices, points[lastId], normal * halfWidth, 1);
    short curMidRightId = addVertex(result.vertices, points[lastId], -normal * halfWidth, 1);
    short curRightId = addVertex(result.vertices, points[lastId], -normal * extHalfWidth, 0);

    addQuadIndices(result.indices, leftId, midLeftId, curLeftId, curMidLeftId);
    addQuadIndices(result.indices, midLeftId, midRightId, curMidLeftId, curMidRightId);
    addQuadIndices(result.indices, midRightId, rightId, curMidRightId, curRightId);

    return result;
}

}
