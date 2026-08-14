#pragma once

#include <tuple>
#include <complex>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "utils/all.h"
#include "scene/scene.h"
#include "writer/video.h"

typedef std::tuple<float, float, float, int> NewtonParam;

struct NewtonScene: public Scene
{
    NewtonScene(int,int);
    void set_param(NewtonParam p);
    std::vector<std::complex<float>> coeffs;
    std::vector<std::complex<float>> roots;
    void update_coefs();
    void update_roots(unsigned n,float x, float y);
    unsigned get_root_pos(std::complex<float> root);
    std::complex<float> get_mouse_coord();
    float click_tol = 0.05;

    virtual void setup_uniforms() override;
    //void animate_to(NewtonParam m1, int frames,VideoWriter& wr, GLFWwindow* window);
    virtual void mouse_drag(float dx, float dy) override;
    virtual bool run(GLFWwindow* window, int) override;
    bool save_video(GLFWwindow* window, int);
    virtual ~NewtonScene();
};

