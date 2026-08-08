#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <span>

#include "ogl/shader.h"

struct Program {
  GLuint id = 0;

  Program(std::span<const uint32_t> vertex_spirv, std::span<const uint32_t> fragment_spirv) {
    id = glCreateProgram();
    if (id == 0)
      throw std::runtime_error{"Failed to create shader program"};

    Shader vertex_shader{GL_VERTEX_SHADER, vertex_spirv};
    Shader fragment_shader{GL_FRAGMENT_SHADER, fragment_spirv};

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

  ~Program() noexcept {
    if (id != 0)
      glDeleteProgram(id);
  }

  operator GLuint() const { return id; }
};
