#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

// Simple passthrough vertex shader
const char* vertexShaderSrc = R"(
#version 450 core
layout(location = 0) in vec2 position;
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

// Simple fragment shader (replace with Mandelbrot shader)
const char* fragmentShaderSrc = R"(
#version 450 core
out vec4 color;
void main() {
    color = vec4(gl_FragCoord.x/800.0, gl_FragCoord.y/600.0, 0.5, 1.0);
}
)";

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << "\n";
    }
    return shader;
}

GLuint createProgram() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        std::cerr << "Program link error: " << log << "\n";
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

int main() {
    const int WIDTH = 800, HEIGHT = 600;
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // hidden window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Dump", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    GLuint program = createProgram();
    glUseProgram(program);

    // Fullscreen quad
    float quadVertices[] = {
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f
    };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<unsigned char> buffer(WIDTH * HEIGHT * 3);
    const int totalFrames = 300;

    for (int frame = 0; frame < totalFrames; ++frame) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();

        glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        std::cout.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    }

    glfwTerminate();
    return 0;
}

