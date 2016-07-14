#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <sstream>
#include <exception>

struct Context {
    GLuint vbo;
    GLuint ibo;

    void create()
    {
        float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f };
        short indices[] = { 0, 1, 2 };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }
};

static Context globalContext;

void render()
{
    std::cout << "Rendering" << std::endl;

    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(0.0, 0.0, 1.0, 1.0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, globalContext.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalContext.ibo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
    glDisableVertexAttribArray(0);
    
    glutSwapBuffers();
}

void initGlut(int* argc, char** argv)
{
    std::cout << "Initing Glut... ";
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GL tests: start");
    glutDisplayFunc(&render);
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
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading OpenGL" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
        return 1;
    }

    globalContext.create();

    glutMainLoop();

    return 0;
}
