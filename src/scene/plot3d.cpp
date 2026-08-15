#include <algorithm>
#include "scene/plot3d.h"
#include "fullscreen_quad_vert.h"

Plot3D::Plot3D(int w, int h, std::span<const uint32_t> fs_spirv, std::string plot_name)
    : Plot(w, h, fullscreen_quad_vert_spirv, fs_spirv),
      cam_yaw(0.85f),
      cam_pitch(0.50f),
      cam_dist(10.0f)
{
    name = plot_name;
}

Plot3D::~Plot3D() {}

void Plot3D::mouse_drag(float dx, float dy) {
    if (width <= 0 || height <= 0) return;
    cam_yaw += (dx / static_cast<float>(width)) * 3.5f;
    cam_pitch = std::clamp(cam_pitch + (dy / static_cast<float>(height)) * 3.5f, -1.45f, 1.45f);
}

void Plot3D::render() {
    if (width <= 0 || height <= 0) return;

    glUseProgram(shader_program);

    upload_common_uniforms();
    glUniform4f(10, params.x, params.y, params.z, params.w);
    glUniform2f(14, cam_yaw, cam_pitch);
    glUniform1f(15, cam_dist * std::max(scale, 0.05f));

    pre_draw();

    glUseProgram(0);
}

void Plot3D::reset_view() {
    Plot::reset_view();
    cam_yaw = 0.85f;
    cam_pitch = 0.50f;
    cam_dist = 10.0f;
}
