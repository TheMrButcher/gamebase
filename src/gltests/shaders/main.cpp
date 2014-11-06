#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/Exception.h>
#include <iostream>
#include <vector>

using namespace gamebase;

namespace {
const std::string SHADERS_PATH = "..\\..\\src\\gltests\\shaders\\";
}

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
        m_matrix[0] = 1; m_matrix[1] = 0;
        m_matrix[2] = 0; m_matrix[3] = 1;
    }

    Matrix2(float m00, float m01, float m10, float m11)
    {
        m_matrix[0] = m00; m_matrix[1] = m01;
        m_matrix[2] = m10; m_matrix[3] = m11;
    }

    Matrix2(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
    }

    Matrix2 operator=(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
        return *this;
    }

    float get(size_t line, size_t col)
    {
        return m_matrix[line * 2 + col];
    }

    float* dataPtr() { return m_matrix; }

private:
    float m_matrix[4];
};

struct Vec2 {
    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}

    float x;
    float y;
};

struct Vec4 {
    Vec4(): x(0), y(0), z(0), w(1) {}
    Vec4(float x, float y, float z, float w = 1): x(x), y(y), z(z), w(w) {}

    float x;
    float y;
    float z;
    float w;
};

class OneColorProgram : public GLProgram {
private:
    static const GLuint POSITION_ID = 0;
public:
    OneColorProgram(): GLProgram(SHADERS_PATH + "shader.vs", SHADERS_PATH + "shader.fs") {}

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
        glUniformMatrix2fv(m_matrixUniformLocation, 1, GL_FALSE, matrix.dataPtr());
        glUniform2f(m_offsetUniformLocation, offset.x, offset.y);
        glUniform4f(m_colorUniformLocation, color.x, color.y, color.z, color.w);

        glEnableVertexAttribArray(POSITION_ID);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        glDrawElements(GL_TRIANGLES, trianglesNum * 3, GL_UNSIGNED_SHORT, NULL);

        glDisableVertexAttribArray(POSITION_ID);
    }

public:
    Matrix2 matrix;
    Vec2 offset;
    Vec4 color;

private:
    GLuint m_matrixUniformLocation;
    GLuint m_offsetUniformLocation;
    GLuint m_colorUniformLocation;
};

struct Context {
    GLuint vbo;
    GLuint ibo;
    OneColorProgram program;

    float angle;
    float scale;
    Vec2 offset;

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

        program.load();
        angle = 0;
        scale = 1;
    }

    Matrix2 projectionMatrix() const
    {
        return Matrix2(
            cosf(angle) * scale, -sinf(angle) * scale,
            sinf(angle) * scale,  cosf(angle) * scale);
    }
};

static Context globalContext;

void render()
{
    std::cout << "Rendering" << std::endl;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    globalContext.program.activate();
    globalContext.program.matrix = globalContext.projectionMatrix();
    globalContext.program.offset = globalContext.offset;
    globalContext.program.color = Vec4(1.0, 1.0, 0.0);
    globalContext.program.draw(globalContext.vbo, globalContext.ibo, 1);
    
    glutSwapBuffers();
}

void processKey(unsigned char key, int, int)
{
    switch (key) {
        case 'a':
            globalContext.offset.x -= 0.05f;
            std::cout << "Key: a, new offset.x=" << globalContext.offset.x << std::endl;
            break;
        case 'd':
            globalContext.offset.x += 0.05f;
            std::cout << "Key: d, new offset.x=" << globalContext.offset.x << std::endl;
            break;
        case 's':
            globalContext.offset.y -= 0.05f;
            std::cout << "Key: s, new offset.y=" << globalContext.offset.y << std::endl;
            break;
        case 'w':
            globalContext.offset.y += 0.05f;
            std::cout << "Key: w, new offset.y=" << globalContext.offset.y << std::endl;
            break;
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
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GL tests: start");
    glutDisplayFunc(&render);
    glutKeyboardFunc(&processKey);
    glutSpecialFunc(&processSpecialKey);
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
    } catch (std::exception& ex)
    {
        std::cerr << "Error while loading OpenGL" << std::endl;
        std::cerr << "Reason: " << ex.what() << std::endl;
        return 1;
    }

    glutMainLoop();

    return 0;
}
