#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <glad/gl.h>
#include <glm/glm.hpp>

#include "scene/plot.h"
#include "grid2d_frag.h"
#include "utils/all.h"

// RAII wrapper for Line VBO & VAO GPU buffers
struct LineMesh
{
    Buffer vbo;
    VertexArray vao;
    size_t count = 0;

    LineMesh() = default;
    LineMesh(const std::vector<glm::vec2>& points);

    void upload_points(const std::vector<glm::vec2>& points);
    void draw(float line_width, const glm::vec4& color) const;
};

// Represents a 2D vertex line series
struct LineSeries
{
    std::string name;
    std::vector<glm::vec2> points;
    LineMesh mesh;
    glm::vec4 color;
    float line_width;
    bool visible;

    LineSeries(const std::string& series_name, const std::vector<glm::vec2>& pts, const glm::vec4& series_color, float width_px = 2.5f);

    void update_points(const std::vector<glm::vec2>& new_pts);
};

// Fast vertex-rasterized 2D Line Plotter derived from Plot
class LinePlot2D : public Plot
{
public:
    Program grid_program;
    Program line_program;
    std::vector<LineSeries> series_list;

    LinePlot2D(int w, int h, std::string plot_name = "line_plot2d");
    virtual ~LinePlot2D() override;

    void add_series(const std::string& name, const std::vector<glm::vec2>& points, const glm::vec4& color = glm::vec4(0.20f, 0.80f, 1.00f, 1.0f), float line_width = 2.5f);
    void add_function(const std::string& name, std::function<float(float)> func, float x_min = -10.0f, float x_max = 10.0f, int num_samples = 1000, const glm::vec4& color = glm::vec4(0.20f, 0.80f, 1.00f, 1.0f), float line_width = 2.5f);

    void toggle_series(size_t index);
    void update_series(size_t index, const std::vector<glm::vec2>& points);

    virtual void render() override;
    virtual void handle_input(GLFWwindow* window) override;

private:
    void init_default_series();
};
