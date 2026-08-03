#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

#include "ogl/shader.h"

struct Program {
  GLuint id = 0;

  Program(const std::string &vertex_src, const std::string &fragment_src) {
    id = glCreateProgram();
    if (id == 0)
      throw std::runtime_error{"Failed to create shader program"};

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

  ~Program() noexcept {
    if (id != 0)
      glDeleteProgram(id);
  }

  operator GLuint() const { return id; }
};
