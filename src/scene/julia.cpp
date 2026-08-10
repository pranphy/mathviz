#include <print>

#include "scene/julia.h"
#include "julia_vert.h"
#include "julia_frag.h"

JuliaScene::JuliaScene(int w, int h) :
    Scene(w, h, julia_vert_spirv, julia_frag_spirv),
    c_real(-0.7f),
    c_imag(0.27015f),
    last_space_pressed(false)
{
    width = w;
    height = h;
    scale = 1.2f; // Slightly zoomed out initially for Julia sets
    x = 0.0f;
    y = 0.0f;
    max_iterations = 100; // More iterations look better for Julia sets
    pasued = false;
    name = "julia_scene";

    x_min_factor = -1.5f;
    x_max_factor =  1.5f;
    y_min_factor = -1.5f;
    y_max_factor =  1.5f;
}

void JuliaScene::setup_uniforms() {
    glUniform2f(5, c_real, c_imag);
}

bool JuliaScene::run(GLFWwindow* window, int mode) {
    if (mode == 0) { // GUI mode
        // Toggle pause/lock state when Spacebar is pressed
        bool current_space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (current_space && !last_space_pressed) {
            pasued = !pasued;
            std::println("Julia Set: {}", pasued ? "Locked/Paused" : "Interactive/Unlocked");
        }
        last_space_pressed = current_space;

        // If not paused, update complex constant C dynamically from the mouse cursor position
        if (!pasued) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            
            // Map cursor coordinates [0, width] -> [-1.5, 1.5]
            if (width > 0 && height > 0) {
                c_real = (static_cast<float>(mx) / width) * 3.0f - 1.5f;
                c_imag = (static_cast<float>(my) / height) * 3.0f - 1.5f;
            }
        }

        render();
        return true;
    }
    
    return false;
}

JuliaScene::~JuliaScene() {}
