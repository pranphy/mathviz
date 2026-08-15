#include "scene/plot2d.h"
#include "fullscreen_quad_vert.h"
#include "grid2d_frag.h"
#include "plot2d_frag.h"

// Modular plot shaders
#include "polynomial_frag.h"
#include "waves_frag.h"
#include "fourier_frag.h"

PlotLayer::PlotLayer(std::string layer_name, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv, glm::vec3 layer_color)
    : name(layer_name),
      shader_program(vs_spirv, fs_spirv),
      color(layer_color),
      params(1.0f, 1.0f, 0.0f, 1.0f),
      visible(true)
{
}

Plot2D::Plot2D(int w, int h, std::string plot_name)
    : Plot(w, h, fullscreen_quad_vert_spirv, plot2d_frag_spirv),
      grid_program(fullscreen_quad_vert_spirv, grid2d_frag_spirv),
      cursor_world(0.0f, 0.0f)
{
    name = plot_name;
    x_min_factor = -10.0f;
    x_max_factor =  10.0f;

    init_default_layers();
}

Plot2D::~Plot2D() {}

void Plot2D::init_default_layers() {
    add_plot(polynomial_frag_spirv, glm::vec3(0.20f, 0.80f, 1.00f), "Polynomial"); // Electric Cyan
    add_plot(waves_frag_spirv,      glm::vec3(1.00f, 0.55f, 0.15f), "Wave");       // Vibrant Orange
    add_plot(fourier_frag_spirv,    glm::vec3(1.00f, 0.90f, 0.25f), "Fourier");    // Bright Yellow
}

void Plot2D::add_plot(std::span<const uint32_t> fs_spirv, glm::vec3 color, std::string layer_name) {
    layers.emplace_back(layer_name, fullscreen_quad_vert_spirv, fs_spirv, color);
}

void Plot2D::set_layer_color(size_t index, glm::vec3 color) {
    if (index < layers.size()) {
        layers[index].color = color;
    }
}

void Plot2D::set_layer_params(size_t index, glm::vec4 layer_params) {
    if (index < layers.size()) {
        layers[index].params = layer_params;
    }
}

void Plot2D::toggle_layer(size_t index) {
    if (index < layers.size()) {
        layers[index].visible = !layers[index].visible;
    }
}

size_t Plot2D::layer_count() const {
    return layers.size();
}

void Plot2D::update_cursor_world() {
    if (width > 0 && height > 0) {
        float norm_x = static_cast<float>(cursor_x) / width;
        float norm_y = 1.0f - static_cast<float>(cursor_y) / height;

        float x_min = x_min_factor * scale + x;
        float x_max = x_max_factor * scale + x;
        float y_min = (x_min_factor / window_aspect) * scale + y;
        float y_max = (x_max_factor / window_aspect) * scale + y;

        cursor_world.x = x_min + norm_x * (x_max - x_min);
        cursor_world.y = y_min + norm_y * (y_max - y_min);
    }
}

void Plot2D::mouse_drag(float dx, float dy) {
    Plot::mouse_drag(dx, dy);
    update_cursor_world();
}

void Plot2D::render() {
    if (width <= 0 || height <= 0) return;

    // Step 1: Draw Background Canvas & Coordinate Grid
    glDisable(GL_BLEND);
    glUseProgram(grid_program);
    upload_common_uniforms();
    glUniform2f(11, cursor_world.x, cursor_world.y);
    pre_draw();

    // Step 2: Sequentially render each graph layer with Alpha Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto& layer : layers) {
        if (!layer.visible) continue;

        glUseProgram(layer.shader_program);
        upload_common_uniforms();

        glUniform3f(9, layer.color.r, layer.color.g, layer.color.b);
        glm::vec4 active_p = params * layer.params;
        glUniform4f(10, active_p.x, active_p.y, active_p.z, active_p.w);
        glUniform2f(11, cursor_world.x, cursor_world.y);

        pre_draw();
    }

    glDisable(GL_BLEND);
    glUseProgram(0);
}

void Plot2D::toggle_cursor() {
    flags ^= 4u;
}

void Plot2D::handle_input(GLFWwindow* window) {
    Plot::handle_input(window);

    // Toggle individual curve layers with number keys 1..5
    static bool key_states[5] = {false, false, false, false, false};
    for (int key = GLFW_KEY_1; key <= GLFW_KEY_5; ++key) {
        size_t idx = static_cast<size_t>(key - GLFW_KEY_1);
        int state = glfwGetKey(window, key);
        if (state == GLFW_PRESS) {
            if (!key_states[idx]) {
                toggle_layer(idx);
                key_states[idx] = true;
            }
        } else if (state == GLFW_RELEASE) {
            key_states[idx] = false;
        }
    }

    // Cursor toggle with 'T'
    static bool t_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!t_pressed) {
            toggle_cursor();
            t_pressed = true;
        }
    } else {
        t_pressed = false;
    }
}
