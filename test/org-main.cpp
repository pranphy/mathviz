#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <iterator>

constexpr int WINDOW_WIDTH  = 3600;
constexpr int WINDOW_HEIGHT = 3240;

std::string read_file(const std::string& file_path)
{
    std::ifstream stream{file_path, std::ios::in};
    if (!stream)
        throw std::runtime_error{"file reading error"};

    return {std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

struct MandelbrotData
{
    float scale;
    float x;
    float y;
    unsigned max_iterations;
};

struct RenderData
{
    GLuint shader_program;
    GLuint vertex_array_object;
};

struct Buffer {
    GLuint id = 0;

    Buffer() {
        glGenBuffers(1, &id);
        if (!id) throw std::runtime_error{"Failed to create buffer"};
    }

    ~Buffer() {
        if (id) glDeleteBuffers(1, &id);
    }

    operator GLuint() const { return id; }
};

struct VertexArray {
    GLuint id = 0;

    VertexArray() {
        glGenVertexArrays(1, &id);
        if (!id) throw std::runtime_error{"Failed to create VAO"};
    }

    ~VertexArray() {
        if (id) glDeleteVertexArrays(1, &id);
    }

    operator GLuint() const { return id; }
};

VertexArray create_rectangle_vao(Buffer& vbo)
{
    constexpr GLfloat rectangle_data[] = {
        -1.f, -1.f, 0.f,
         1.f, -1.f, 0.f,
         1.f,  1.f, 0.f,
         1.f,  1.f, 0.f,
        -1.f,  1.f, 0.f,
        -1.f, -1.f, 0.f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_data), rectangle_data, GL_STATIC_DRAW);

    VertexArray vao;
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}


struct Shader {
    GLuint id = 0;

    Shader(GLenum type, const std::string& source) {
        id = glCreateShader(type);
        if (id == 0) throw std::runtime_error{"Failed to create shader"};

        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        GLint ok = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) throw std::runtime_error{"Shader compilation failed"};
    }

    ~Shader() {
        if (id != 0)
            glDeleteShader(id);
    }

    // Allow implicit conversion to GLuint
    operator GLuint() const { return id; }
};

struct Program {
    GLuint id = 0;

    Program(const std::string& vertex_src, const std::string& fragment_src)
    {
        id = glCreateProgram();
        if (id == 0) throw std::runtime_error{"Failed to create shader program"};

        Shader vertex_shader{GL_VERTEX_SHADER, vertex_src};
        Shader fragment_shader{GL_FRAGMENT_SHADER, fragment_src};

        glAttachShader(id, vertex_shader);
        glAttachShader(id, fragment_shader);

        glLinkProgram(id);

        GLint ok = GL_FALSE;
        glGetProgramiv(id, GL_LINK_STATUS, &ok);
        if (ok == GL_FALSE)
            throw std::runtime_error{"Shader program linking failed"};

        glDetachShader(id, vertex_shader);
        glDetachShader(id, fragment_shader);
    }

    ~Program() {
        if (id != 0)
            glDeleteProgram(id);
    }

    operator GLuint() const { return id; }
};






void handle_input(GLFWwindow* window, MandelbrotData& m, bool& running)
{
    glfwPollEvents();
    if (glfwWindowShouldClose(window)) running = false;

    float step = 0.01f * m.scale;

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) m.y += step;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) m.y -= step;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) m.x -= step;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) m.x += step;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m.scale -= step;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m.scale += step;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) m.max_iterations++;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && m.max_iterations > 0)
        m.max_iterations--;
}

void render(const MandelbrotData& m, const RenderData& r)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(r.shader_program);

    glUniform1f(0, WINDOW_WIDTH);
    glUniform1f(1, WINDOW_HEIGHT);

    glUniform2f(2,
        -2.f * m.scale + m.x,
         1.f * m.scale + m.x
    );

    glUniform2f(3,
        -1.f * m.scale + m.y,
         1.f * m.scale + m.y
    );

    glUniform1ui(4, m.max_iterations);

    glBindVertexArray(r.vertex_array_object);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
}

int main()
{
    if (!glfwInit()) {
        std::cerr << "GLFW initialization error\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MandelbrotGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation error\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    try {
        Buffer rectangle_buffer;
        VertexArray rectangle_vao = create_rectangle_vao(rectangle_buffer);

        Program shader_program{
            read_file("res/mandelbrot_shader.vs"),
            read_file("res/mandelbrot_shader.fs")
        };

        MandelbrotData mandelbrot{1.f, 0.f, 0.f, 30};
        RenderData render_data{shader_program, rectangle_vao};

        bool running = true;
        while (running) {
            handle_input(window, mandelbrot, running);
            render(mandelbrot, render_data);
            glfwSwapBuffers(window);
        }
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


