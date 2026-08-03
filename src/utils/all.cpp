
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

