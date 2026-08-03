#pragma once

#include <filesystem>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <stdexcept>

[[nodiscard]] static std::string read_file(const std::filesystem::path& file_path)
{
    std::ifstream stream{file_path, std::ios::in | std::ios::binary};
    if (!stream) {
        throw std::runtime_error(std::format("file reading error: {}", file_path.string()));
    }
    return {std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

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

