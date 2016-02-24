#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <sstream>
#include <exception>

float angle = 0;
float scale = 1;
float vertices[] = { -200, -200, 200, -200, 200, 200, -200, 200 };
short indices[] = { 0, 1, 2, 0, 2, 3 };
GLuint vbo = 0;
GLuint ibo = 0;

void render()
{
    if (vbo)
        glDeleteBuffers(1, &vbo);
    if (ibo)
        glDeleteBuffers(1, &ibo);

    float transformedVertices[8];
    for (int i = 0; i < 4; ++i)
    {
        transformedVertices[i * 2]     = scale * (vertices[i * 2] * cos(angle) - vertices[i * 2 + 1] * sin(angle)) / 800.0;
        transformedVertices[i * 2 + 1] = scale * (vertices[i * 2] * sin(angle) + vertices[i * 2 + 1] * cos(angle)) / 600.0;
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transformedVertices), transformedVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}

void processButton(int key, int, int)
{
    switch (key) {
    case GLUT_KEY_UP: scale += 0.1; break;
    case GLUT_KEY_DOWN: scale -= 0.1; break;
    case GLUT_KEY_LEFT: angle += 3.14 / 10; break;
    case GLUT_KEY_RIGHT: angle -= 3.14 / 10; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Test");
    glutDisplayFunc(&render);
    glutSpecialFunc(&processButton);
    GLenum result = glewInit();
    if (result != GLEW_OK)
        return 1;
    glutMainLoop();
    return 0;
}
