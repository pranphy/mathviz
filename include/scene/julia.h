#pragma once

#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/all.h"
#include "scene/scene.h"
#include "ogl/program.h"

struct JuliaScene : public Scene
{
    std::vector<unsigned char> buffer;
    Buffer rectangle_buffer;
    VertexArray rectangle_vao;

    Program shader_program;

    // Julia constant parameters: C = c_real + i * c_imag
    float c_real;
    float c_imag;
    bool last_space_pressed;

    JuliaScene(int w, int h);
    virtual void setup_uniforms() const override;
    virtual bool run(GLFWwindow* window, int mode) override;
    virtual void set_resolution(int width, int height) override;
    virtual ~JuliaScene();
};
