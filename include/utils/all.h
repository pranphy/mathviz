#pragma once

#include <filesystem>
#include <string>
#include <stdexcept>
#include <vector>
#include <glm/glm.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

constexpr float pi = std::numbers::pi;

std::string read_file(const std::filesystem::path& file_path);
std::string read_shader_file(const std::filesystem::path& file_path);

struct Buffer {
    GLuint id = 0;

    Buffer() {
        glGenBuffers(1, &id);
        if (!id) throw std::runtime_error{"Failed to create buffer"};
    }

    ~Buffer() noexcept {
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

    ~VertexArray() noexcept {
        if (id) glDeleteVertexArrays(1, &id);
    }

    operator GLuint() const { return id; }
};


void create_rectangle_vao(Buffer& vbo, VertexArray& vao);
void create_points_vao(Buffer& vbo, VertexArray& vao, const std::vector<glm::vec4>& data);
