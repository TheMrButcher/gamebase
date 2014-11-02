#include <stdafx.h>
#include <gamebase/graphics/Init.h>
#include "State.h"
#include "src/core/Config.h"
#include <gamebase/utils/Exception.h>
#include <iostream>

namespace gamebase {
namespace {
const std::string DEFAULT_CONFIG_NAME = "config.json";

class WindowInitializer {
public:
    WindowInitializer(const std::string& name, int posX, int posY)
        : m_name(name)
        , m_posX(posX)
        , m_posY(posY)
    {}

    void operator()(int width, int height)
    {
        glutInitWindowSize(width, height);
        glutInitWindowPosition(m_posX, m_posY);
        glutCreateWindow(m_name.c_str());
    }

private:
    std::string m_name;
    int m_posX;
    int m_posY;
};

void enterGameMode(int width, int height)
{
    std::ostringstream ss;
    ss << width << "x" << height << ":32@60";
    glutGameModeString(ss.str().c_str());
    glutEnterGameMode();
}

void initGlut(int* argc, char** argv)
{
    std::cout << "Initing Glut... ";
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
}

void initGlew()
{
    std::cout << "Initing Glew... ";
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::ostringstream ss;
        ss << "Error while loading Glew: " << glewGetErrorString(result);
        THROW_EX() << ss.str();
    }
    std::cout << "Done" << std::endl;
}

template <typename InitFunc>
void init(int* argc, char** argv,
    int width, int height, InitFunc initFunc)
{
    loadConfig(DEFAULT_CONFIG_NAME);
    initGlut(argc, argv);
    initFunc(width, height);
    std::cout << "Done" << std::endl;
    initGlew();
    initState(width, height);
}
}

void initWindowMode(int* argc, char** argv,
    int width, int height, const std::string& name,
    int posX, int posY)
{
    init(argc, argv, width, height, WindowInitializer(name, posX, posY));
}

void initGameMode(int* argc, char** argv, int width, int height)
{
    init(argc, argv, width, height, &enterGameMode);
}

}