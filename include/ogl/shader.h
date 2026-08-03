#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

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

    ~Shader() noexcept {
        if (id != 0)
            glDeleteShader(id);
    }

    // Allow implicit conversion to GLuint
    operator GLuint() const { return id; }
};

