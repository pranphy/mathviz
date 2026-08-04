#include "scene/julia.h"

static const std::string julia_vs = R"(#version 460 core
layout(location = 0) in vec3 vertex_position;
void main()
{
    gl_Position = vec4(vertex_position, 1.0);
}
)";

static const std::string julia_fs = R"(#version 460 core
layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;
layout(location = 5) uniform vec2 julia_c;

out vec4 pixel_color;

const vec3 color_map[] = {
    vec3(0.0,  0.0,  0.0),
    vec3(0.26, 0.18, 0.06),
    vec3(0.1,  0.03, 0.1),
    vec3(0.04, 0.0,  0.18),
    vec3(0.02, 0.02, 0.29),
    vec3(0.0,  0.03, 0.39),
    vec3(0.05, 0.17, 0.54),
    vec3(0.09, 0.32, 0.69),
    vec3(0.22, 0.49, 0.82),
    vec3(0.52, 0.71, 0.9),
    vec3(0.82, 0.92, 0.97),
    vec3(0.94, 0.91, 0.75),
    vec3(0.97, 0.79, 0.37),
    vec3(1.0,  0.67, 0.0),
    vec3(0.8,  0.5,  0.0),
    vec3(0.6,  0.34, 0.0),
    vec3(0.41, 0.2,  0.01)
};

void main()
{
    // Z is mapped from the screen coordinates
    vec2 Z = vec2(gl_FragCoord.x * (area_w.y - area_w.x) / rect_width  + area_w.x,
                  gl_FragCoord.y * (area_h.y - area_h.x) / rect_height + area_h.x);
    vec2 C = julia_c;
    uint iteration = 0;
    float z_sq_dist = 0.0;

    while (iteration < max_iterations)
    {
        float x = Z.x * Z.x - Z.y * Z.y + C.x;
        float y = 2.0 * Z.x * Z.y        + C.y;

        z_sq_dist = x * x + y * y;
        if (z_sq_dist > 4.0)
            break;

        Z.x = x;
        Z.y = y;

        ++iteration;
    }

    if (iteration == max_iterations)
    {
        pixel_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Smooth coloring using hardware-accelerated log2
        float log_zn = log2(z_sq_dist) * 0.5;
        float nu = log2(log_zn);
        float smooth_iter = float(iteration) + 1.0 - nu;

        // Map the smooth iteration count to color map range (17 colors)
        float color_pos = mod(smooth_iter * 0.2, float(color_map.length()));
        
        int idx1 = int(floor(color_pos));
        int idx2 = (idx1 + 1) % color_map.length();
        float fract_part = fract(color_pos);

        vec3 final_color = mix(color_map[idx1], color_map[idx2], fract_part);
        pixel_color = vec4(final_color, 1.0);
    }
}
)";

JuliaScene::JuliaScene(int w, int h) :
    buffer(h * w * 3),
    rectangle_buffer{},
    rectangle_vao{},
    shader_program{julia_vs, julia_fs},
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

    x_min_factor = -1.5f;
    x_max_factor =  1.5f;
    y_min_factor = -1.5f;
    y_max_factor =  1.5f;

    create_rectangle_vao(rectangle_buffer, rectangle_vao);
    render_data = RenderData{shader_program, rectangle_vao};
}

void JuliaScene::set_resolution(int w, int h) {
    width = w;
    height = h;
    buffer.resize(w * h * 3);
}

void JuliaScene::setup_uniforms() const {
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
