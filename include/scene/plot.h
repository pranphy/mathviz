#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <span>
#include <string>

#include "scene/scene.h"
#include "writer/video.h"

// Base class for mathematical plotting scenes
class Plot : public Scene
{
public:
    glm::vec4 params; // Custom user parameters (a, b, c, d)
    uint32_t flags;   // Bitmask: 1 = grid, 2 = axes, 4 = cursor, 8 = neon glow

    Plot(int w, int h, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv);
    virtual ~Plot() override;

    // Common Parameter & State Manipulations
    void set_parameters(float a, float b, float c, float d);
    void set_mode(int mode);
    void toggle_grid();
    void toggle_axes();
    void toggle_glow();
    virtual void reset_view();

    // Centralized uniform upload helper for all math shaders
    virtual void upload_common_uniforms() override;

    // Scene lifecycle & rendering
    virtual void render() = 0;
    virtual void mouse_drag(float dx, float dy) override;
    virtual void handle_input(GLFWwindow* window);
    virtual bool run(GLFWwindow* window, int mode) override;
    virtual void set_resolution(int width, int height) override;

    // Video Recording
    bool save_video(GLFWwindow* window, int mode);
};
