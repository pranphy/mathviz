#pragma once

#include <vector>
#include <tuple>
#include <print>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/all.h"
#include "scene/scene.h"
#include "video/writer.h"
#include "ogl/render.h"
#include "ogl/program.h"

typedef std::tuple<float, float, float, int> MandelbrotParam;

struct MandelbrotScene: public Scene
{
    int width, height;
    //std::string vertex_shader;
    //std::string fragment_shader;
    std::vector<unsigned char> buffer;
    Buffer rectangle_buffer;
    VertexArray rectangle_vao;

    Program shader_program;
    RenderData render_data;

    MandelbrotScene(int,int);
    void set_param(MandelbrotParam p);

    virtual void render() const override;
    void move_along_path(MandelbrotScene m1, MandelbrotScene m2, int frames,VideoWriter& wr, GLFWwindow* window);
    void animate_to(MandelbrotParam m1, int frames,VideoWriter& wr, GLFWwindow* window);
    virtual bool run(GLFWwindow* window, int) override;
    virtual ~MandelbrotScene();
};

