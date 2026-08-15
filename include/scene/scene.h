#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "ogl/program.h"
#include "utils/all.h"

#include <span>

class Scene {
public:
    float scale, x, y;
    int width, height;
    float window_aspect;
    std::vector<unsigned char> buffer;

    int max_iterations;
    int param = 3;

    double cursor_x, cursor_y;
    bool is_dragging;
    bool paused = false;
    bool is_animated = false;
    float u_time = 0.0f; // Global simulation / animation time provided by App

    std::string name;

    Program shader_program;
    Buffer rectangle_buffer;
    VertexArray rectangle_vao;

    // View bounds configuration (factors multiplied by scale)
    float x_min_factor = -2.0f;
    float x_max_factor = 1.0f;

    Scene(int w, int h, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv);
    virtual ~Scene() = 0;

    void render();
    void set_dragging(bool state);
    void set_mouse_pos(double x, double y);
    void set_time(float t);
    virtual void upload_common_uniforms();
    void save_scene();
    virtual void set_resolution(int width, int height);

    void step_right();
    void step_left();
    void step_up();
    void step_down();
    void zoom_in();
    void zoom_out();
    void iterate_up();
    void iterate_down();
    void increment_param();
    void decrement_param();
    virtual void mouse_drag(float dx, float dy);
    virtual bool run(GLFWwindow *, int) = 0;

    // Subclass hooks for custom states and custom uniforms
    virtual void pre_draw();
    virtual void post_draw();
    virtual void setup_uniforms() {}
};
