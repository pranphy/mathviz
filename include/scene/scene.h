#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

struct Scene{
    float scale,x,y;
    int max_iterations;
    bool pasued = false;
    void step_right();
    void step_left();
    void step_up();
    void step_down();
    void zoom_in();
    void zoom_out();
    void iterate_up();
    void iterate_down();
    virtual bool run(GLFWwindow*,int) = 0;
    virtual void render() const = 0;
    virtual ~Scene()=0;
};
