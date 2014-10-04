#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/Exception.h>
#include <boost/optional.hpp>
#include <iostream>
#include <vector>

using namespace gamebase;

class Shader {
public:
    Shader(GLenum type, const std::string& name)
        : m_type(type)
        , m_name(name)
        , m_id(0)
    {}

    const std::string& name() const { return m_name; }
    GLuint id() const { return m_id; }

    GLuint load()
    {
        std::string shaderStr = loadTextFile(m_name);
        m_id = glCreateShader(m_type);
        if (m_id == 0)
            THROW_EX() << "Can't create shader " << m_name;
        
        const char* data = &shaderStr.front();
        GLint len = (GLint)shaderStr.length();
        glShaderSource(m_id, 1, &data, &len);
        glCompileShader(m_id);

        GLint success;
        glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLchar buf[1024];
            glGetShaderInfoLog(m_id, sizeof(buf), NULL, buf);
            THROW_EX() << "Error while compiling shader " << m_name << ": " << buf;
        }

        return m_id;
    }

private:
    GLenum m_type;
    std::string m_name;
    GLuint m_id;
};

class GLProgram {
public:
    GLProgram(
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName)
        : m_vertexShader(GL_VERTEX_SHADER, vertexShaderName)
        , m_fragmentShader(GL_FRAGMENT_SHADER, fragmentShaderName)
    {}

    GLuint id() const { return m_id; }

    virtual GLuint load()
    {
        m_id = glCreateProgram();
        if (m_id == 0)
            THROW_EX() << "Can't create program";

        glAttachShader(m_id, m_vertexShader.load());
        glAttachShader(m_id, m_fragmentShader.load());
        glLinkProgram(m_id);

        GLint success;
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success) {
            GLchar buf[1024];
            glGetProgramInfoLog(m_id, sizeof(buf), NULL, buf);
            THROW_EX() << "Error while linking program: " << buf;
        }
        
        return m_id;
    }

    virtual void activate() { glUseProgram(m_id); }

private:
    Shader m_vertexShader;
    Shader m_fragmentShader;
    GLuint m_id;
};

GLuint locateUniform(GLuint programID, const std::string& name)
{
    GLuint uniformLocation = glGetUniformLocation(programID, name.c_str());
    if (uniformLocation == 0xFFFFFFFF)
        THROW_EX() << "Can't locate uniform " << name << " in program " << programID;
    return uniformLocation;
}

class Matrix2 {
public:
    Matrix2()
    {
        m_matrix[0] = 1; m_matrix[2] = 0;
        m_matrix[1] = 0; m_matrix[3] = 1;
    }

    Matrix2(float m00, float m01, float m10, float m11)
    {
        m_matrix[0] = m00; m_matrix[2] = m01;
        m_matrix[1] = m10; m_matrix[3] = m11;
    }

    Matrix2(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
    }

    Matrix2& operator=(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
        return *this;
    }

    void set(size_t line, size_t col, float value)
    {
        m_matrix[col * 2 + line] = value;
    }

    float get(size_t line, size_t col) const
    {
        return m_matrix[col * 2 + line];
    }

    float* dataPtr() { return m_matrix; }

    float determinant() const
    {
        return m_matrix[0] * m_matrix[3] - m_matrix[1] * m_matrix[2];
    }

    Matrix2& transpose()
    {
        std::swap(m_matrix[1], m_matrix[2]);
        return *this;
    }

    Matrix2 transposed() const
    {
        return Matrix2(
            m_matrix[0], m_matrix[1],
            m_matrix[2], m_matrix[3]);
    }

    Matrix2& operator+=(const Matrix2& other)
    {
        for (size_t i = 0; i < 4; ++i)
            m_matrix[i] += other.m_matrix[i];
        return *this;
    }

    Matrix2& operator-=(const Matrix2& other)
    {
        for (size_t i = 0; i < 4; ++i)
            m_matrix[i] -= other.m_matrix[i];
        return *this;
    }

    Matrix2& operator*=(const Matrix2& other)
    {
        Matrix2 m(*this);
        m_matrix[0] = m.m_matrix[0] * other.m_matrix[0] + m.m_matrix[2] * other.m_matrix[1];
        m_matrix[1] = m.m_matrix[1] * other.m_matrix[0] + m.m_matrix[3] * other.m_matrix[1];
        m_matrix[2] = m.m_matrix[0] * other.m_matrix[2] + m.m_matrix[2] * other.m_matrix[3];
        m_matrix[3] = m.m_matrix[1] * other.m_matrix[2] + m.m_matrix[3] * other.m_matrix[3];
        return *this;
    }

    Matrix2 operator-()
    {
        Matrix2 result;
        for (size_t i = 0; i < 4; ++i)
            result.m_matrix[i] = -m_matrix[i];
        return result;
    }

private:
    float m_matrix[4];
};

Matrix2 operator+(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result += m2;
    return result;
}

Matrix2 operator-(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result -= m2;
    return result;
}

Matrix2 operator*(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result *= m2;
    return result;
}


class ScalingMatrix2 : public Matrix2 {
public:
    ScalingMatrix2(float scale)
        : Matrix2(scale, 0, 0, scale)
    {}

    ScalingMatrix2(float scaleX, float scaleY)
        : Matrix2(scaleX, 0, 0, scaleY)
    {}
};


class RotationMatrix2 : public Matrix2 {
public:
    RotationMatrix2(float angle)
        : Matrix2(
            cosf(angle), -sinf(angle),
            sinf(angle),  cosf(angle))
    {}
};


struct Vec2 {
    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}

    float length() const { return std::sqrtf(x * x + y * y); }

    bool isZero() const { return x == 0 && y == 0; }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float num)
    {
        x *= num;
        y *= num;
        return *this;
    }

    Vec2& normalize()
    {
        if (isZero())
            THROW_EX() << "Can't normalize zero-sized vector";
        return operator*=(1.0f / length());
    }

    float x;
    float y;
};

Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x + v2.x, v1.y + v2.y);
}

Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x - v2.x, v1.y - v2.y);
}

Vec2 operator*(const Vec2& v, float num)
{
    return Vec2(v.x * num, v.y * num);
}

float dot(const Vec2& v1, const Vec2& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float cross(const Vec2& v1, const Vec2& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

Vec2 rotate90(const Vec2& v)
{
    return Vec2(-v.y, v.x);
}

Vec2 minVec(const Vec2& v1, const Vec2& v2)
{
    return Vec2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
}

Vec2 maxVec(const Vec2& v1, const Vec2& v2)
{
    return Vec2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
}

Vec2 operator*(const Matrix2& m, const Vec2& v)
{
    return Vec2(
        m.get(0, 0) * v.x + m.get(0, 1) * v.y,
        m.get(1, 0) * v.x + m.get(1, 1) * v.y);
}


struct Transform2 {
    Transform2(const Matrix2& matrix = Matrix2(), const Vec2& offset = Vec2())
        : matrix(matrix)
        , offset(offset)
    {}

    Transform2& operator*=(const Transform2& other)
    {
        matrix = other.matrix * matrix;
        offset = other.matrix * offset + other.offset;
        return *this;
    }

    Matrix2 matrix;
    Vec2 offset;
};

Transform2 operator*(const Transform2& t1, const Transform2& t2)
{
    Transform2 result(t1);
    result *= t2;
    return result;
}


class ScalingTransform2 : public Transform2 {
public:
    ScalingTransform2(float scale)
        : Transform2(ScalingMatrix2(scale))
    {}

    ScalingTransform2(float scaleX, float scaleY)
        : Transform2(ScalingMatrix2(scaleX, scaleY))
    {}
};


class RotationTransform2 : public Transform2 {
public:
    RotationTransform2(float angle)
        : Transform2(RotationMatrix2(angle))
    {}
};


class ShiftTransform2 : public Transform2 {
public:
    ShiftTransform2(float x, float y)
        : Transform2(Matrix2(), Vec2(x, y))
    {}

    ShiftTransform2(const Vec2& offset)
        : Transform2(Matrix2(), offset)
    {}
};


struct BoundingBox {
    BoundingBox()
        : bottomLeft(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max())
        , topRight(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min())
    {}

    BoundingBox(const Vec2& v)
        : bottomLeft(v)
        , topRight(v)
    {}

    BoundingBox(const Vec2& v1, const Vec2& v2)
        : bottomLeft(minVec(v1, v2))
        , topRight(maxVec(v1, v2))
    {}

    bool isValid() const
    {
        return bottomLeft.x <= topRight.x && bottomLeft.y <= topRight.y;
    }

    float width() const { return topRight.x - bottomLeft.x; }
    float height() const { return topRight.y - bottomLeft.y; }
    float area() const { return width() * height(); }

    bool contains(const Vec2& v) const
    {
        return bottomLeft.x <= v.x
            && bottomLeft.y <= v.y
            && v.x <= topRight.x
            && v.y <= topRight.y;
    }

    BoundingBox& extend(const Vec2& v)
    {
        bottomLeft = minVec(bottomLeft, v);
        topRight = maxVec(topRight, v);
        return *this;
    }

    BoundingBox uniteWith(const BoundingBox& other) const
    {
        BoundingBox result;
        result.bottomLeft = minVec(bottomLeft, other.bottomLeft);
        result.topRight = maxVec(topRight, other.topRight);
        return result;
    }

    BoundingBox intersectWith(const BoundingBox& other) const
    {
        BoundingBox result;
        result.bottomLeft = maxVec(bottomLeft, other.bottomLeft);
        result.topRight = minVec(topRight, other.topRight);
        return result;
    }

    bool intersects(const BoundingBox& other) const
    {
        return bottomLeft.x <= other.topRight.x
            && bottomLeft.y <= other.topRight.y
            && topRight.x >= other.bottomLeft.x
            && topRight.y >= other.bottomLeft.y;
    }

    Vec2 bottomLeft;
    Vec2 topRight;
};


struct Segment {
    Segment(const Vec2& a, const Vec2& b)
        : a(a)
        , b(b)
    {}

    BoundingBox boundingBox() const
    {
        return BoundingBox(a, b);
    }

    bool isVertical() const { return b.x - a.x == 0; }

    float k() const
    {
        if (isVertical())
            THROW_EX() << "Can't count k for vertical segment";
        Vec2 v = b - a;
        return v.y / v.x;
    }

    float c() const
    {
        return a.y - k() * a.x;
    }

    float getX(float y) const
    {
        if (isVertical())
            return a.x;
        return (y - c()) / k();
    }

    float substitute(float x) const
    {
        if (isVertical())
            return a.y;
        return k() * x + c();
    }

    Vec2 a;
    Vec2 b;
};

boost::optional<Vec2> vertAndInclinedSegmentsIntersection(
    const Segment& vertSegment, const Segment& segment)
{
    float y = segment.substitute(vertSegment.a.x);
    std::pair<float, float> minMaxY = std::minmax(vertSegment.a.y, vertSegment.b.y);
    if (minMaxY.first <= y && y <= minMaxY.second)
        return Vec2(vertSegment.a.x, y);
    return boost::none;
}

boost::optional<Vec2> intersection(const Segment& s1, const Segment& s2)
{
    BoundingBox box1 = s1.boundingBox();
    BoundingBox box2 = s2.boundingBox();
    BoundingBox boxIntersection = box1.intersectWith(box2);
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
    if (boxIntersection.contains(result))
        return result;
    return boost::none;
}


struct PolylineMesh {
    std::vector<float> vertices;
    std::vector<short> indices;
};

short addVertex(
    std::vector<float>& vertices,
    const Vec2& v,
    float distance)
{
    short index = static_cast<short>(vertices.size() / 3);
    vertices.push_back(v.x);
    vertices.push_back(v.y);
    vertices.push_back(distance);
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
        return std::make_pair(*intrs, boost::none);
    return std::make_pair(cur1, cur2);
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
        leftId = addVertex(result.vertices, points[0] + normal * extHalfWidth, 0);
        midLeftId = addVertex(result.vertices, points[0] + normal * halfWidth, 1);
        midRightId = addVertex(result.vertices, points[0] - normal * halfWidth, 1);
        rightId = addVertex(result.vertices, points[0] - normal * extHalfWidth, 0);
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

        short curLeftId = addVertex(result.vertices, extCapLeft.first, 0);
        short curMidLeftId = addVertex(result.vertices, capLeft.first, 1);
        short curMidRightId = addVertex(result.vertices, capRight.first, 1);
        short curRightId = addVertex(result.vertices, extCapRight.first, 0);

        addQuadIndices(result.indices, leftId, midLeftId, curLeftId, curMidLeftId);
        addQuadIndices(result.indices, midLeftId, midRightId, curMidLeftId, curMidRightId);
        addQuadIndices(result.indices, midRightId, rightId, curMidRightId, curRightId);

        if ((capLeft.second || capRight.second) && !(capLeft.second && capRight.second)) {
            if (capLeft.second) {
                short curLeftId2 = addVertex(result.vertices, *extCapLeft.second, 0);
                short curMidLeftId2 = addVertex(result.vertices, *capLeft.second, 1);
                addTriangleIndices(result.indices, curMidLeftId, curMidLeftId2, curMidRightId);
                addQuadIndices(result.indices, curMidLeftId, curMidLeftId2, curLeftId, curLeftId2);
                curLeftId = curLeftId2;
                curMidLeftId = curMidLeftId2;
            } else {
                short curMidRightId2 = addVertex(result.vertices, *capRight.second, 1);
                short curRightId2 = addVertex(result.vertices, *extCapRight.second, 0);
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

    short curLeftId = addVertex(result.vertices, points[lastId] + normal * extHalfWidth, 0);
    short curMidLeftId = addVertex(result.vertices, points[lastId] + normal * halfWidth, 1);
    short curMidRightId = addVertex(result.vertices, points[lastId] - normal * halfWidth, 1);
    short curRightId = addVertex(result.vertices, points[lastId] - normal * extHalfWidth, 0);

    addQuadIndices(result.indices, leftId, midLeftId, curLeftId, curMidLeftId);
    addQuadIndices(result.indices, midLeftId, midRightId, curMidLeftId, curMidRightId);
    addQuadIndices(result.indices, midRightId, rightId, curMidRightId, curRightId);

    return result;
}


struct Vec4 {
    Vec4(): x(0), y(0), z(0), w(1) {}
    Vec4(float x, float y, float z, float w = 1): x(x), y(y), z(z), w(w) {}

    float x;
    float y;
    float z;
    float w;
};

class LineProgram : public GLProgram {
private:
    static const GLuint POSITION_ID = 0;
    static const GLuint DISTANCE_ID = 1;
public:
    LineProgram(): GLProgram("shader.vs", "shader.fs") {}

    GLuint load()
    {
        GLuint id = GLProgram::load();
        m_matrixUniformLocation = locateUniform(id, "uMatrix");
        m_offsetUniformLocation = locateUniform(id, "uOffset");
        m_colorUniformLocation = locateUniform(id, "uColor");
        return id;
    }

    void draw(GLuint vbo, GLuint ibo, size_t trianglesNum)
    {
        glUniformMatrix2fv(m_matrixUniformLocation, 1, GL_FALSE, transform.matrix.dataPtr());
        glUniform2f(m_offsetUniformLocation, transform.offset.x, transform.offset.y);
        glUniform4f(m_colorUniformLocation, color.x, color.y, color.z, color.w);


        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glEnableVertexAttribArray(POSITION_ID);
        glEnableVertexAttribArray(DISTANCE_ID);
        glVertexAttribPointer(POSITION_ID, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glVertexAttribPointer(DISTANCE_ID, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 2));

        glDrawElements(GL_TRIANGLES, trianglesNum * 3, GL_UNSIGNED_SHORT, NULL);

        glDisableVertexAttribArray(POSITION_ID);
        glDisableVertexAttribArray(DISTANCE_ID);
    }

public:
    Transform2 transform;
    Vec4 color;

private:
    GLuint m_matrixUniformLocation;
    GLuint m_offsetUniformLocation;
    GLuint m_colorUniformLocation;
};

struct Context {
    GLuint vbo;
    GLuint ibo;
    size_t trianglesNum;
    LineProgram program;

    float angle;
    float scale;
    Vec2 offset;

    void create()
    {
        glViewport(0, 0, 1280, 1024);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Vec2 points[] = {
            Vec2(-200.0f, -200.0f),
            Vec2( 200.0f, -200.0f),
            Vec2( 200.0f,  200.0f),
            Vec2(-200.0f,  200.0f),
            Vec2(-200.0f, -200.0f) };

        /*Vec2 points[] = {
            Vec2(   0.0f, -200.0f),
            Vec2(   0.0f,    0.0f),
            Vec2( 200.0f,    0.0f),
            Vec2(   0.0f,  200.0f),
            Vec2(-200.0f,  200.0f),
            Vec2(-200.0f, -200.0f) };*/

        PolylineMesh mesh = buildPolylineMesh(
            points, sizeof(points) / sizeof(Vec2), 10.0f);

        /*float vertices[] = {
            -2.0f, -300.0f, 0.0f,
            -2.0f,  300.0f, 0.0f,
             0.0f, -300.0f, 1.0f,
             0.0f,  300.0f, 1.0f,
             2.0f, -300.0f, 0.0f,
             2.0f,  300.f, 0.0f};
        short indices[] = {
            0, 1, 2,
            1, 2, 3,
            2, 3, 4,
            3, 4, 5};*/

        program.load();
        angle = 0;
        scale = 1;
        
        trianglesNum = mesh.indices.size() / 3;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), &mesh.vertices.front(), GL_STATIC_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(short), &mesh.indices.front(), GL_STATIC_DRAW);
    }

    Transform2 projectionTransform() const
    {
        return
            RotationTransform2(angle) *
            ShiftTransform2(offset) *
            ScalingTransform2(1.0f / 640, 1.0f / 512) *
            ScalingTransform2(scale);

        /*return Matrix2(
            cosf(angle) * scale / 640, -sinf(angle) * scale / 640,
            sinf(angle) * scale / 512,  cosf(angle) * scale / 512);*/
    }
};

static Context globalContext;

void render()
{
    std::cout << "Rendering" << std::endl;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    globalContext.program.activate();
    globalContext.program.transform = globalContext.projectionTransform();
    globalContext.program.color = Vec4(1.0, 1.0, 0.0);
    globalContext.program.draw(globalContext.vbo, globalContext.ibo, globalContext.trianglesNum);
    
    glutSwapBuffers();
}

void processKey(unsigned char key, int, int)
{
    switch (key) {
        case 'a':
            globalContext.offset.x -= 10.0f;
            std::cout << "Key: a, new offset.x=" << globalContext.offset.x << std::endl;
            break;
        case 'd':
            globalContext.offset.x += 10.0f;
            std::cout << "Key: d, new offset.x=" << globalContext.offset.x << std::endl;
            break;
        case 's':
            globalContext.offset.y -= 10.0f;
            std::cout << "Key: s, new offset.y=" << globalContext.offset.y << std::endl;
            break;
        case 'w':
            globalContext.offset.y += 10.0f;
            std::cout << "Key: w, new offset.y=" << globalContext.offset.y << std::endl;
            break;
        case 27:
        case 'e':
            glutLeaveGameMode();
            glutLeaveMainLoop();
        default:
            return;
    }
    glutPostRedisplay();
}

void processSpecialKey(int key, int, int)
{
    switch (key) {
        case GLUT_KEY_LEFT:
            globalContext.angle += 0.05f;
            std::cout << "Key: left, new angle=" << globalContext.angle << std::endl;
            break;
        case GLUT_KEY_RIGHT:
            globalContext.angle -= 0.05f;
            std::cout << "Key: right, new angle=" << globalContext.angle << std::endl;
            break;
        case GLUT_KEY_DOWN:
            globalContext.scale -= 0.05f;
            std::cout << "Key: down, new scale=" << globalContext.scale << std::endl;
            break;
        case GLUT_KEY_UP:
            globalContext.scale += 0.05f;
            std::cout << "Key: up, new scale=" << globalContext.scale << std::endl;
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

void initGlut(int* argc, char** argv)
{
    std::cout << "Initing Glut... ";
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    //glutInitWindowSize(400, 400);
    //glutInitWindowPosition(100, 100);
    //glutCreateWindow("GL tests: start");
    glutGameModeString("1280x1024:32@60");
    glutEnterGameMode();
    std::cout << "Done" << std::endl;
}

void initGlew()
{
    std::cout << "Initing Glew... ";
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::ostringstream ss;
        ss << "Error while loading Glew: " << glewGetErrorString(result);
        throw std::exception(ss.str().c_str());
    }
    std::cout << "Done" << std::endl;
}

int main(int argc, char** argv)
{
    try {
        initGlut(&argc, argv);
        initGlew();
        globalContext.create();
        glutDisplayFunc(&render);
        glutKeyboardFunc(&processKey);
        glutSpecialFunc(&processSpecialKey);
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading OpenGL" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
        return 1;
    }

    glutMainLoop();

    return 0;
}
