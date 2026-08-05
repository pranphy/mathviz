#pragma once

#include <vector>
#include <tuple>
#include <print>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/all.h"
#include "scene/scene.h"
#include "writer/video.h"
#include "ogl/render.h"
#include "ogl/program.h"

typedef std::tuple<float, float, float, int> MandelbrotParam;

struct MandelbrotScene: public Scene
{
    //std::string vertex_shader;
    //std::string fragment_shader;
    //std::vector<unsigned char> buffer;
    Buffer rectangle_buffer;
    VertexArray rectangle_vao;

    Program shader_program;

    MandelbrotScene(int,int);
    void set_param(MandelbrotParam p);

    void animate_to(MandelbrotParam m1, int frames,VideoWriter& wr, GLFWwindow* window);
    virtual bool run(GLFWwindow* window, int) override;
    bool save_video(GLFWwindow* window, int);
    virtual void set_resolution(int width, int height) override;
    virtual ~MandelbrotScene();
};

