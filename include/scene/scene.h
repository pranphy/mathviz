#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "ogl/render.h"

struct Scene{
    float scale,x,y;
    int max_iterations;
    int width, height;
    std::vector<unsigned char> buffer;
    bool pasued = false;
    std::string name;

    // Common OpenGL rendering data
    RenderData render_data;

    // View bounds configuration (factors multiplied by scale)
    float x_min_factor = -2.0f;
    float x_max_factor =  1.0f;
    float y_min_factor = -1.0f;
    float y_max_factor =  1.0f;

    Scene(int w, int h);

    void step_right();
    void step_left();
    void step_up();
    void step_down();
    void zoom_in();
    void zoom_out();
    void iterate_up();
    void iterate_down();
    void save_scene();
    virtual void set_resolution(int width, int height);
    virtual bool run(GLFWwindow*,int) = 0;

    // Non-virtual rendering logic
    void render() const;

    // Subclass hook for binding custom uniforms
    virtual void setup_uniforms() const {}

    virtual ~Scene()=0;
};

