#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gamebase/graphics/Init.h>
#include <gamebase/graphics/OneColorProgram.h>
#include <gamebase/graphics/LineProgram.h>
#include <gamebase/geom/PolylineMesh.h>
#include <gamebase/engine/CameraMover.h>
#include <gamebase/utils/Exception.h>
#include <boost/optional.hpp>
#include <boost/math/constants/constants.hpp>
#include <iostream>
#include <vector>
#include <stdint.h>

using namespace gamebase;

Vec2 triangle[] = {
    Vec2(-100, -100),
    Vec2(0, 100),
    Vec2(100, -100)};

class Timer {
public:
    Timer()
    {
        start();
    }

    void start()
    {
        m_startTicks = GetTickCount64();
    }

    double time() const
    {
        return (GetTickCount64() - m_startTicks) / 1000.0;
    }

private:
    uint64_t m_startTicks;
};

bool isPointInTriangle(const Vec2& point,
    const Vec2& p1, const Vec2& p2, const Vec2& p3)
{
    float a = cross(point - p1, p2 - p1);
    float b = cross(point - p2, p3 - p2);
    float c = cross(point - p3, p1 - p3);
    return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

std::vector<Vec2> createCircle(const Vec2& center, float radius, size_t pointsNum)
{
    std::vector<Vec2> result;
    float step = 2.0f * boost::math::constants::pi<float>() / pointsNum;
    for (size_t i = 0; i <= pointsNum; ++i)
        result.push_back(center + Vec2(
            radius * std::cosf(step * i), radius * std::sinf(step * i)));
    return result;
}

struct Buffers {
    Buffers() {}
    Buffers(const VertexBuffer& vbo, const IndexBuffer& ibo)
        : vbo(vbo)
        , ibo(ibo)
    {}

    VertexBuffer vbo;
    IndexBuffer ibo;
};

Buffers createCircleBuffers(float radius)
{
    size_t pointsNum = std::max(
        static_cast<uint32_t>(2.0f * boost::math::constants::pi<float>() * radius / 30), 8u);
    std::vector<Vec2> circle = createCircle(Vec2(), radius, pointsNum);
    std::cout << "Created circle with radius=" << radius << ", number of points=" << pointsNum << std::endl;
    PolylineMesh mesh = buildPolylineMesh(circle, 4.0);
    return Buffers(VertexBuffer(mesh.vertices), IndexBuffer(mesh.indices));
}


class Counter {
public:
    Counter(const std::string& message = "Counter", double period = 1.0)
        : m_message(message)
        , m_period(period)
    {
        start();
    }

    void start()
    {
        m_timer.start();
        m_counts = 0;
    }

    void touch()
    {
        ++m_counts;
        if (m_timer.time() >= m_period) {
            std::cout << m_message << ": " << m_counts << std::endl;
            start();
        }
    }

private:
    std::string m_message;
    double m_period;
    Timer m_timer;
    size_t m_counts;
};

struct Context {
    Camera camera;
    CameraMover cameraMover;
    Buffers triangleBuffers;
    Buffers triangleContourBuffers;
    Counter fpsCounter;
    Timer gameTimer;
    Vec2 mousePosition;
    bool isMouseButtonPressed;
    bool startedMousePressing;

    void create();
};

Context globalContext;

class FlyingTriangle {
public:
    FlyingTriangle()
        : m_scale(1)
        , m_color(1, 1, 1)
        , m_currentColor(1, 1, 1)
        , m_trajectoryRadius(0)
        , m_trajectoryAngle(0)
        , m_trajectoryAngleSpeed(0)
        , m_rotationAngle(0)
        , m_rotationAngleSpeed(0)
        , m_drawLines(false)
    {}

    void setScale(float scale) { m_scale = scale; }
    void setColor(const Color& color) { m_color = color; m_currentColor = color; }
    void setTrajectoryRadius(float radius) { m_trajectoryRadius = radius; }
    void setTrajectoryAngle(float angle) { m_trajectoryAngle = angle; }
    void setTrajectoryPeriod(float period)
    {
        m_trajectoryAngleSpeed = 2.0f * boost::math::constants::pi<float>() / period;
    }

    void setRotationAngle(float angle) { m_rotationAngle = angle; }
    void setRotationPeriod(float period)
    {
        m_rotationAngleSpeed = 2.0f * boost::math::constants::pi<float>() / period;
    }

    void addChild(std::shared_ptr<FlyingTriangle> child)
    {
        m_children.push_back(child);
    }

    void load()
    {
        if (m_trajectoryRadius != 0)
            m_trajectoryBuffers = createCircleBuffers(m_trajectoryRadius);

        for (auto it = m_children.begin(); it != m_children.end(); ++it)
            (*it)->load();
    }

    void move(float time)
    {
        m_trajectoryAngle += m_trajectoryAngleSpeed * time;
        m_rotationAngle += m_rotationAngleSpeed * time;

        for (auto it = m_children.begin(); it != m_children.end(); ++it)
            (*it)->move(time);
    }

    Transform2 position() const
    {
        Vec2 position = RotationMatrix2(m_trajectoryAngle) * Vec2(m_trajectoryRadius, 0);
        return ShiftTransform2(position);
    }

    void draw(const Transform2& globalPosition) const
    {
        if (m_trajectoryRadius != 0) {
            const LineProgram& program = lineProgram();
            program.activate();
            program.transform = globalPosition;
            program.color = Color(0.3f, 1.0f, 0.3f);
            program.draw(m_trajectoryBuffers.vbo, m_trajectoryBuffers.ibo);
        }

        Transform2 fullPosition = position() * globalPosition;
        Transform2 fullTransform = RotationTransform2(m_rotationAngle)
            * ScalingTransform2(m_scale)
            * fullPosition;
        const OneColorProgram& program = oneColorProgram();
        program.activate();
        program.transform = fullTransform;
        program.color = m_currentColor;
        program.draw(
            globalContext.triangleBuffers.vbo,
            globalContext.triangleBuffers.ibo);

        if (m_drawLines) {
            const LineProgram& program = lineProgram();
            program.activate();
            program.transform = fullTransform;
            program.color = Color(1.0f, 0.3f, 0.3f);
            program.draw(
                globalContext.triangleContourBuffers.vbo,
                globalContext.triangleContourBuffers.ibo);
        }

        for (auto it = m_children.begin(); it != m_children.end(); ++it)
            (*it)->draw(fullPosition);
    }

    FlyingTriangle* find(const Vec2& point, const Transform2& globalPosition)
    {
        Transform2 fullPosition = position() * globalPosition;
        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            FlyingTriangle* result = (*it)->find(point, fullPosition);
            if (result)
                return result;
        }
        Transform2 transform = RotationTransform2(m_rotationAngle)
            * ScalingTransform2(m_scale)
            * fullPosition;
        if (isPointInTriangle(point, transform * triangle[0],
            transform * triangle[1], transform * triangle[2]))
            return this;
        return nullptr;
    }

    void setLighted(bool enabled)
    {
        m_currentColor = enabled ? m_color * 1.15f : m_color;
    }

    void setFocused(bool enabled)
    {
        m_drawLines = enabled;
    }

private:
    float m_scale;
    Color m_color;
    Color m_currentColor;
    float m_trajectoryRadius;
    float m_trajectoryAngle;
    float m_trajectoryAngleSpeed;
    float m_rotationAngle;
    float m_rotationAngleSpeed;
    bool m_drawLines;

    std::vector<std::shared_ptr<FlyingTriangle>> m_children;

    Buffers m_trajectoryBuffers;
};

std::shared_ptr<FlyingTriangle> triangleSystem;
FlyingTriangle* lightedTriangle = nullptr;
FlyingTriangle* focusedTriangle = nullptr;

VertexBuffer createTriangleVertexBuffer(
    float x0, float y0, float x1, float y1, float x2, float y2)
{
    std::vector<float> vertices;
    vertices.push_back(x0); vertices.push_back(y0);
    vertices.push_back(x1); vertices.push_back(y1);
    vertices.push_back(x2); vertices.push_back(y2);
    return VertexBuffer(vertices);
}

VertexBuffer createTriangleVertexBuffer(
    const Vec2& p0, const Vec2& p1, const Vec2& p2)
{
    return createTriangleVertexBuffer(
        p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
}

void Context::create()
{
    cameraMover = CameraMover(&camera);
    short indices[3] = { 0, 1, 2 };
    triangleBuffers.vbo = createTriangleVertexBuffer(
        triangle[0], triangle[1], triangle[2]);
    triangleBuffers.ibo = IndexBuffer(indices, 3);

    {
        std::vector<Vec2> contour(triangle, triangle + 3);
        contour.push_back(triangle[0]);
        PolylineMesh mesh = buildPolylineMesh(contour, 5.0);
        triangleContourBuffers.vbo = VertexBuffer(mesh.vertices);
        triangleContourBuffers.ibo = IndexBuffer(mesh.indices);
    }

    fpsCounter = Counter("Frames per 10 seconds", 10.0);
    isMouseButtonPressed = false;
    startedMousePressing = false;
    
    triangleSystem.reset(new FlyingTriangle());
    triangleSystem->setColor(Color(0.7f, 0.7f, 0));
    triangleSystem->setRotationPeriod(16);

    std::shared_ptr<FlyingTriangle> planet1(new FlyingTriangle());
    planet1->setScale(0.5f);
    planet1->setColor(Color(0.7f, 0.7f, 1.0f));
    planet1->setTrajectoryRadius(400);
    planet1->setTrajectoryPeriod(6);
    planet1->setRotationPeriod(3);
    triangleSystem->addChild(planet1);

    std::shared_ptr<FlyingTriangle> moon1(new FlyingTriangle());
    moon1->setScale(0.15f);
    moon1->setTrajectoryRadius(150);
    moon1->setTrajectoryPeriod(2);
    moon1->setRotationPeriod(1);
    planet1->addChild(moon1);

    std::shared_ptr<FlyingTriangle> planet2(new FlyingTriangle());
    planet2->setScale(0.3f);
    planet2->setColor(Color(0.8f, 1.0f, 0.6f));
    planet2->setTrajectoryRadius(700);
    planet2->setTrajectoryAngle(2.0f / 3 * boost::math::constants::pi<float>()); 
    planet2->setTrajectoryPeriod(10);
    planet2->setRotationPeriod(8.5f);
    triangleSystem->addChild(planet2);

    triangleSystem->load();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gameTimer.start();
}

void processKey(unsigned char key, int, int)
{
    if (key == 27 || key == 'e') {
        glutLeaveGameMode();
        glutLeaveMainLoop();
        return;
    }
    
    globalContext.cameraMover.processKey(key);
}

void processKeyUp(unsigned char key, int, int)
{
    globalContext.cameraMover.processKeyUp(key);
}

void processSpecialKey(int key, int, int)
{
    globalContext.cameraMover.processSpecialKey(key);
}

void processSpecialKeyUp(int key, int, int)
{
    globalContext.cameraMover.processSpecialKeyUp(key);
}

void proccessMouseMotion(int x, int y)
{
    globalContext.mousePosition = Vec2(
        static_cast<float>(x), static_cast<float>(y));
}

void proccessPassiveMouseMotion(int x, int y)
{
    globalContext.mousePosition = Vec2(
        static_cast<float>(x), static_cast<float>(y));
}

void processMouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        std::cout << "Left mouse button: ";
        if (state == GLUT_DOWN)
            std::cout << "down";
        else
            std::cout << "up";
        std::cout << std::endl;
        globalContext.isMouseButtonPressed = state == GLUT_DOWN;
        if (globalContext.isMouseButtonPressed)
            globalContext.startedMousePressing = true;
    }
    globalContext.mousePosition = Vec2(
        static_cast<float>(x), static_cast<float>(y));
}

Vec2 convert(const Vec2& mouseCoords)
{
    Size scrnSize = screenSize();
    return projectionTransform()
        * Vec2(2.0f * mouseCoords.x - scrnSize.width, scrnSize.height - 2.0f * mouseCoords.y);
}

void processMove()
{
    globalContext.cameraMover.move();
    float time = static_cast<float>(globalContext.gameTimer.time());
    globalContext.gameTimer.start();
    triangleSystem->move(time);
}

void processInput()
{
    Vec2 mouseXY = convert(globalContext.mousePosition);
    FlyingTriangle* current = triangleSystem->find(
        mouseXY, globalContext.camera.transform());
    if (current != lightedTriangle) {
        if (lightedTriangle)
            lightedTriangle->setLighted(false);
        lightedTriangle = current;
        if (lightedTriangle)
            lightedTriangle->setLighted(true);
    }

    if (globalContext.isMouseButtonPressed
        && globalContext.startedMousePressing
        && current
        && current != focusedTriangle) {
        if (focusedTriangle)
            focusedTriangle->setFocused(false);
        focusedTriangle = current;
        focusedTriangle->setFocused(true);
    }
    globalContext.startedMousePressing = false;
}

void render()
{
    globalContext.fpsCounter.touch();
    processMove();
    processInput();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    try {
        triangleSystem->draw(globalContext.camera.transform());
    } catch (std::exception& ex)
    {
        std::cerr << "Error while rendering" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
    }
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    try {
        initWindowMode(&argc, argv, 1000, 800);
        //initGameMode(&argc, argv, 1280, 1024);
        globalContext.create();
        glutDisplayFunc(&render);
        glutKeyboardFunc(&processKey);
        glutKeyboardUpFunc(&processKeyUp);
        glutSpecialFunc(&processSpecialKey);
        glutSpecialUpFunc(&processSpecialKeyUp);
        glutMotionFunc(&proccessMouseMotion);
        glutPassiveMotionFunc(&proccessPassiveMouseMotion);
        glutMouseFunc(&processMouseButton);
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading OpenGL" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
        return 1;
    }

    glutMainLoop();

    return 0;
}
