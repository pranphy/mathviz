#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <span>
#include <vector>
#include <string>

struct Shader {
    GLuint id = 0;

    Shader(GLenum type, std::span<const uint32_t> spirv_code) {
        id = glCreateShader(type);
        if (id == 0) throw std::runtime_error{"Failed to create shader"};

        glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv_code.data(), spirv_code.size_bytes());
        glSpecializeShader(id, "main", 0, nullptr, nullptr);

        GLint ok = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            GLint log_len = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
            if (log_len > 0) {
                std::vector<char> log(log_len);
                glGetShaderInfoLog(id, log_len, nullptr, log.data());
                throw std::runtime_error(std::string("Shader specialization failed: ") + log.data());
            }
            throw std::runtime_error{"Shader specialization failed"};
        }
    }

    ~Shader() noexcept {
        if (id != 0)
            glDeleteShader(id);
    }

    // Allow implicit conversion to GLuint
    operator GLuint() const { return id; }
};

