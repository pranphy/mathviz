#pragma once

#include <tuple>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/all.h"
#include "scene/scene.h"
#include "writer/video.h"

typedef std::tuple<float, float, float, int> MandelbrotParam;

struct MandelbrotScene: public Scene
{
    MandelbrotScene(int,int);
    void set_param(MandelbrotParam p);

    void animate_to(MandelbrotParam m1, int frames,VideoWriter& wr, GLFWwindow* window);
    virtual bool run(GLFWwindow* window, int) override;
    bool save_video(GLFWwindow* window, int);
    virtual ~MandelbrotScene();
};

