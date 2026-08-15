#pragma once

#include <span>
#include <string>
#include <vector>
#include <memory>
#include "scene/plot.h"

// Represents an individual mathematical graph layer on a 2D coordinate plane
struct PlotLayer
{
    std::string name;
    Program shader_program;
    glm::vec3 color;
    glm::vec4 params;
    bool visible;

    PlotLayer(std::string layer_name, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv, glm::vec3 layer_color);
};

// 2D Multi-Layer Composite Plotter derived from Plot
class Plot2D : public Plot
{
public:
    Program grid_program;
    std::vector<PlotLayer> layers;
    glm::vec2 cursor_world;

    Plot2D(int w, int h, std::string plot_name = "plot2d");
    virtual ~Plot2D() override;

    void add_plot(std::span<const uint32_t> fs_spirv, glm::vec3 color = glm::vec3(0.20f, 0.80f, 1.00f), std::string name = "curve");
    void set_layer_color(size_t index, glm::vec3 color);
    void set_layer_params(size_t index, glm::vec4 layer_params);
    void toggle_layer(size_t index);
    size_t layer_count() const;

    virtual void render() override;
    virtual void mouse_drag(float dx, float dy) override;
    virtual void handle_input(GLFWwindow* window) override;

    void toggle_cursor();

private:
    void update_cursor_world();
    void init_default_layers();
};
