#include <print>
#include <format>
#include <algorithm>

#include "scene/plot.h"

Plot::Plot(int w, int h, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv)
    : Scene(w, h, vs_spirv, fs_spirv),
      params(1.0f, 1.0f, 0.0f, 1.0f),
      flags(1u | 2u | 4u | 8u) // Grid (1), Axes (2), Cursor (4), Neon Glow (8)
{
    name = "plot";
    scale = 1.0f;
    x = 0.0f;
    y = 0.0f;
    window_aspect = static_cast<float>(width) / height;

    max_iterations = 6;
    param = 0;
    is_animated = true;
}

Plot::~Plot() {}

void Plot::set_resolution(int w, int h) {
    width = w;
    height = h;
    buffer.resize(w * h * 3);
    window_aspect = static_cast<float>(w) / h;
}

void Plot::set_parameters(float a, float b, float c, float d) {
    params = glm::vec4(a, b, c, d);
}

void Plot::set_mode(int mode) {
    param = mode;
}

void Plot::toggle_grid() {
    flags ^= 1u;
}

void Plot::toggle_axes() {
    flags ^= 2u;
}

void Plot::toggle_glow() {
    flags ^= 8u;
}

void Plot::reset_view() {
    scale = 1.0f;
    x = 0.0f;
    y = 0.0f;
}

void Plot::upload_common_uniforms() {
    Scene::upload_common_uniforms();
    glUniform1ui(12, flags);
}

void Plot::mouse_drag(float dx, float dy) {
    if (width <= 0 || height <= 0) return;
    float aspect = static_cast<float>(width) / height;
    float x_span = (x_max_factor - x_min_factor) * scale;
    float y_span = ((x_max_factor - x_min_factor) / aspect) * scale;
    x -= (dx / static_cast<float>(width)) * x_span;
    y += (dy / static_cast<float>(height)) * y_span;
}

void Plot::handle_input(GLFWwindow* window) {
    // Toggle Neon Glow with 'V'
    static bool v_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!v_pressed) {
            toggle_glow();
            v_pressed = true;
        }
    } else {
        v_pressed = false;
    }

    // Toggle Grid with 'B'
    static bool b_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!b_pressed) {
            toggle_grid();
            b_pressed = true;
        }
    } else {
        b_pressed = false;
    }

    // Toggle Axes with 'N'
    static bool n_pressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        if (!n_pressed) {
            toggle_axes();
            n_pressed = true;
        }
    } else {
        n_pressed = false;
    }

    // Parameter tuning
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        params.x += 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        params.x -= 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        params.y += 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        params.y -= 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        params.z += 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        params.z -= 0.02f;
    }

    // Reset view with 'R'
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        reset_view();
    }
}

bool Plot::save_video(GLFWwindow* window, int /* unused */) {
    int fps = 30;
    VideoWriter wr(std::format("./{}_output.mp4", name), width, height, fps);
    int duration_sec = 5;
    int total_frames = fps * duration_sec;

    for (int i = 0; i < total_frames; ++i) {
        render();
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        wr.push_frame(buffer.data());
        glfwSwapBuffers(window);
    }
    return false;
}

bool Plot::run(GLFWwindow* window, int mode) {
    if (mode == 0) { // GUI mode
        handle_input(window);
        render();
        return true;
    } else {
        return save_video(window, mode);
    }
}
