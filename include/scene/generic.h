#pragma once

#include <tuple>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "scene/scene.h"

typedef std::tuple<float, float, float, int> GenericParam;

struct GenericScene: public Scene
{
    GenericScene(int,int);
    void set_param(GenericParam p);

    virtual void setup_uniforms() override;
    virtual bool run(GLFWwindow* window, int) override;
    bool save_video(GLFWwindow* window, int);
    virtual void set_resolution(int width, int height) override;
    virtual ~GenericScene();
};

