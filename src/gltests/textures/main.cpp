#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gamebase/graphics/Init.h>
#include <gamebase/graphics/OneColorProgram.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/engine/CameraMover.h>
#include <gamebase/utils/Counter.h>
#include <Magick++/Blob.h>
#include <Magick++/Image.h>
#include <Magick++/Functions.h>
#include <boost/math/constants/constants.hpp>
#include <iostream>

using namespace gamebase;

namespace {

const std::string TEXTURE_SHADERS_PATH = "..\\..\\src\\gltests\\textures\\";
const std::string IMAGES_PATH = "..\\..\\resources\\images\\cosmic_space\\";

}

struct Image {
    Image(std::vector<uint8_t>&& data, const Size& size)
        : data(std::move(data))
        , size(size)
    {}

    const std::vector<uint8_t> data;
    const Size size;
};

std::unique_ptr<Image> whiteRectImage()
{
    const size_t SIZE = 16;
    std::vector<uint8_t> data(SIZE * SIZE * 4, 255);
    return std::unique_ptr<Image>(new Image(
        std::move(data), Size(SIZE, SIZE)));
}

class Texture {
public:
    Texture() {}

    Texture(const Image& image)
    {
        m_size = image.size;
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.size.width, image.size.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, &image.data.front());
    }

    GLuint id() const { return m_id; }
    const Size& size() const { return m_size; }

    void bind() const
    {
        if (m_size.width == 0 || m_size.height == 0)
            THROW_EX() << "Can't bind empty Texture";
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

private:
    GLuint m_id;
    Size m_size;
};

class TextureProgram : public ProgramBase {
public:
    TextureProgram()
        : ProgramBase(
            "TextureProgram",
            TEXTURE_SHADERS_PATH + "Texture.vs",
            TEXTURE_SHADERS_PATH + "Texture.fs")
    {
        m_attrs.add(GLAttributes::TextureCoords, 2);
    }

public:
    mutable Texture texture;

protected:
    virtual void loadUniforms() const override
    {
        ProgramBase::loadUniforms();
        texture.bind();
    }
};

struct Context {
    Camera camera;
    CameraMover cameraMover;
    Counter fpsCounter;
    Timer gameTimer;
    Vec2 mousePosition;
    bool isMouseButtonPressed;
    bool startedMousePressing;
    std::unique_ptr<TextureProgram> textureProgram;

    void create();
};

Context globalContext;

class IObject {
public:
    virtual ~IObject() {}
};

class IDrawable : public virtual IObject {
public:
    virtual void loadResources() = 0;

    virtual void draw(const Transform2& globalPosition) const = 0;
};

class IPositionable : public virtual IObject {
public:
    virtual Transform2 position() const = 0;

    virtual Transform2 transform() const = 0;
};

class IMovable : public IPositionable {
public:
    virtual void move(float time) = 0;
};

class IFindable : public virtual IObject {
public:
    virtual IFindable* find(
        const Vec2& point, const Transform2& globalPosition) = 0;
};

class FilledTriangle : public IDrawable {
public:
    FilledTriangle(const IPositionable* position)
        : m_drawPosition(position)
        , m_color(1, 1, 1)
    {}

    void setColor(const Color& color) { m_color = color; }
    void setPoints(const Vec2* points) { memcpy(m_points, points, sizeof(m_points)); }

    virtual void loadResources() override
    {
        m_buffers = createTriangleBuffers(m_points);
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        const OneColorProgram& program = oneColorProgram();
        program.activate();
        program.transform = m_drawPosition->transform() * globalPosition;
        program.color = m_color;
        program.draw(m_buffers.vbo, m_buffers.ibo);
    }

private:
    const IPositionable* m_drawPosition;
    Color m_color;
    Vec2 m_points[3];
    GLBuffers m_buffers;
};

class RotatingObject : public IMovable {
public:
    RotatingObject()
        : m_trajectoryRadius(0)
        , m_trajectoryAngle(0)
        , m_trajectoryAngleSpeed(0)
        , m_rotationAngle(0)
        , m_rotationAngleSpeed(0)
    {}

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

    virtual Transform2 position() const override
    {
        Vec2 position = RotationMatrix2(m_trajectoryAngle) * Vec2(m_trajectoryRadius, 0);
        return ShiftTransform2(position);
    }

    virtual Transform2 transform() const override
    {
        return RotationTransform2(m_rotationAngle)
            * position();
    }

    virtual void move(float time) override
    {
        m_trajectoryAngle += m_trajectoryAngleSpeed * time;
        m_rotationAngle += m_rotationAngleSpeed * time;
    }

private:
    float m_trajectoryRadius;
    float m_trajectoryAngle;
    float m_trajectoryAngleSpeed;
    float m_rotationAngle;
    float m_rotationAngleSpeed;
};

class FlyingTriangle : public RotatingObject, public FilledTriangle {
public:
    FlyingTriangle()
        : RotatingObject()
        , FilledTriangle(this)
    {}
};

class TextureRect : public IDrawable {
public:
    TextureRect(const IPositionable* position)
        : m_drawPosition(position)
    {}

    void setTexture(const Texture& texture) { m_texture = texture; }
    void setRect(const BoundingBox& rect) { m_rect = rect; }

    virtual void loadResources() override
    {
        m_buffers = createTextureRectBuffers(m_rect);
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        const TextureProgram& program = *globalContext.textureProgram;
        program.activate();
        program.transform = m_drawPosition->transform() * globalPosition;
        program.texture = m_texture;
        program.draw(m_buffers.vbo, m_buffers.ibo);
    }

private:
    const IPositionable* m_drawPosition;
    
    BoundingBox m_rect;
    GLBuffers m_buffers;
    Texture m_texture;
};

class WhiteRect : public TextureRect {
public:
    WhiteRect(const IPositionable* position)
        : TextureRect(position)
    {}

    virtual void loadResources() override
    {
        TextureRect::loadResources();
        setTexture(Texture(*whiteRectImage()));
    }
};

class FlyingRect : public RotatingObject, public WhiteRect {
public:
    FlyingRect()
        : RotatingObject()
        , WhiteRect(this)
    {}
};

class SpaceObject : public RotatingObject, public TextureRect {
public:
    SpaceObject()
        : RotatingObject()
        , TextureRect(this)
    {}
};


std::unique_ptr<Image> loadImage(const std::string& fname)
{
    try {
        Magick::Image image(fname);
        Magick::Blob blob;
        image.write(&blob, "RGBA");
        std::vector<uint8_t> data(blob.length());
        memcpy(&data.front(), blob.data(), blob.length());
        std::cout << "Loaded image " << fname
            << ", width: " << image.size().width()
            << ", height: " << image.size().height()
            << ", buffer size: " << blob.length() << std::endl;
        return std::unique_ptr<Image>(new Image(std::move(data),
            Size(image.size().width(), image.size().height())));
    } catch (const Magick::Exception& ex) {
        std::cerr << "Error while trying to load image: " << fname
            << ", reason: " << ex.what() << std::endl;
        return whiteRectImage();
    }
}

class ObjectsSystem : public IMovable, public IDrawable {
public:
    ObjectsSystem(const std::shared_ptr<IObject>& mainObject)
    {
        m_objects.push_back(mainObject);
        if (auto movable = dynamic_cast<IMovable*>(mainObject.get()))
            m_movableObjects.push_back(movable);
        m_position = dynamic_cast<IPositionable*>(mainObject.get());
        m_mainDrawable = dynamic_cast<IDrawable*>(mainObject.get());
    }

    void addChild(const std::shared_ptr<IObject>& object)
    {
        m_objects.push_back(object);
        if (auto movable = dynamic_cast<IMovable*>(object.get()))
            m_movableObjects.push_back(movable);
        if (auto drawable = dynamic_cast<IDrawable*>(object.get()))
            m_drawableObjects.push_back(drawable);
    }

    virtual Transform2 position() const override
    {
        return m_position ? m_position->position() : Transform2();
    }

    virtual Transform2 transform() const override
    {
        return m_position ? m_position->transform() : Transform2();
    }

    virtual void move(float time) override
    {
        for (auto it = m_movableObjects.begin(); it != m_movableObjects.end(); ++it)
            (*it)->move(time);
    }

    virtual void loadResources() override
    {
        if (m_mainDrawable)
            m_mainDrawable->loadResources();

        for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
            (*it)->loadResources();
    }

    virtual void draw(const Transform2& globalPosition) const override
    {
        if (m_mainDrawable)
            m_mainDrawable->draw(globalPosition);

        for (auto it = m_drawableObjects.begin(); it != m_drawableObjects.end(); ++it)
            (*it)->draw(m_position->position() * globalPosition);
    }

private:
    std::vector<std::shared_ptr<IObject>> m_objects;
    IPositionable* m_position;
    IDrawable* m_mainDrawable;
    std::vector<IMovable*> m_movableObjects;
    std::vector<IDrawable*> m_drawableObjects;
};

std::shared_ptr<ObjectsSystem> objects;

void Context::create()
{
    cameraMover = CameraMover(&camera);
    fpsCounter = Counter("Frames per 10 seconds", 10.0);
    isMouseButtonPressed = false;
    startedMousePressing = false;

    std::shared_ptr<SpaceObject> sun(new SpaceObject);
    {
        sun->setRect(BoundingBox(Vec2(-240, -240), Vec2(240, 240)));
        sun->setTexture(Texture(*loadImage(IMAGES_PATH + "sun.png")));
        sun->setRotationPeriod(15);
    }

    std::shared_ptr<FlyingTriangle> planet(new FlyingTriangle);
    {
        Vec2 points[] = { Vec2(0, 70), Vec2(-120, -80), Vec2(60, -35) };
        planet->setPoints(points);
        planet->setColor(Color(0.3f, 1, 0));
        planet->setRotationPeriod(6);
        planet->setTrajectoryRadius(400);
        planet->setTrajectoryPeriod(10);
    }

    std::shared_ptr<FlyingRect> planet2(new FlyingRect);
    {
        planet2->setRect(BoundingBox(Vec2(-50, -50), Vec2(50, 50)));
        planet2->setRotationPeriod(4);
        planet2->setTrajectoryRadius(600);
        planet2->setTrajectoryPeriod(13);
    }

    std::shared_ptr<SpaceObject> planet3(new SpaceObject);
    {
        planet3->setRect(BoundingBox(Vec2(-80, -80), Vec2(80, 80)));
        planet3->setRotationPeriod(7);
        planet3->setTexture(Texture(*loadImage(IMAGES_PATH + "mars.png")));
        planet3->setTrajectoryRadius(800);
        planet3->setTrajectoryPeriod(20);
    }

    objects.reset(new ObjectsSystem(sun));
    objects->addChild(planet);
    objects->addChild(planet2);
    objects->addChild(planet3);
    objects->loadResources();

    textureProgram.reset(new TextureProgram);
    textureProgram->load();

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
    objects->move(time);
}

void processInput()
{
    Vec2 mouseXY = convert(globalContext.mousePosition);
    /*FlyingTriangle* current = triangleSystem->find(
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
    }*/
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
        objects->draw(globalContext.camera.transform());
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
        Magick::InitializeMagick(argv[0]);
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
        std::cerr << "Error while loading" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
        return 1;
    }

    glutMainLoop();

    return 0;
}
