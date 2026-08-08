
#include <filesystem>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <stdexcept>

#include "utils/all.h"

std::string read_file(const std::filesystem::path& file_path)
{
    std::ifstream stream{file_path, std::ios::in | std::ios::binary};
    if (!stream) {
        throw std::runtime_error(std::format("file reading error: {}", file_path.string()));
    }
    return {std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

std::string read_shader_file(const std::filesystem::path& file_path)
{
    std::ifstream stream{file_path};
    if (!stream) {
        throw std::runtime_error(std::format("shader file reading error: {}", file_path.string()));
    }

    std::string content;
    std::string line;
    std::filesystem::path base_dir = file_path.parent_path();

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.starts_with("#include")) {
            size_t first_quote = line.find_first_of("\"<");
            size_t last_quote = line.find_last_of("\">");
            if (first_quote != std::string::npos && last_quote != std::string::npos && last_quote > first_quote) {
                std::string include_name = line.substr(first_quote + 1, last_quote - first_quote - 1);
                std::filesystem::path include_path = base_dir / include_name;
                content += read_shader_file(include_path) + "\n";
                continue;
            }
        }
        content += line + "\n";
    }
    return content;
}


void create_rectangle_vao(Buffer& vbo, VertexArray& vao)
{
    constexpr std::array<GLfloat, 18> rectangle_data = {
        -1.f, -1.f, 0.f,
         1.f, -1.f, 0.f,
         1.f,  1.f, 0.f,
         1.f,  1.f, 0.f,
        -1.f,  1.f, 0.f,
        -1.f, -1.f, 0.f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(rectangle_data.size() * sizeof(GLfloat)),
                 rectangle_data.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

