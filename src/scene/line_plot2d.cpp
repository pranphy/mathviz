#include <cmath>
#include <algorithm>

#include "scene/line_plot2d.h"
#include "fullscreen_quad_vert.h"
#include "grid2d_frag.h"
#include "line_plot_vert.h"
#include "line_plot_frag.h"

// LineMesh RAII implementation
LineMesh::LineMesh(const std::vector<glm::vec2>& points) {
    upload_points(points);
}

void LineMesh::upload_points(const std::vector<glm::vec2>& points) {
    count = points.size();
    if (count == 0) return;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec2), points.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineMesh::draw(float line_width, const glm::vec4& color) const {
    if (count == 0) return;
    glLineWidth(line_width);
    glUniform4f(9, color.r, color.g, color.b, color.a);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(count));
    glBindVertexArray(0);
}

// LineSeries implementation
LineSeries::LineSeries(const std::string& series_name, const std::vector<glm::vec2>& pts, const glm::vec4& series_color, float width_px)
    : name(series_name),
      points(pts),
      mesh(pts),
      color(series_color),
      line_width(width_px),
      visible(true)
{
}

void LineSeries::update_points(const std::vector<glm::vec2>& new_pts) {
    points = new_pts;
    mesh.upload_points(points);
}

// LinePlot2D implementation
LinePlot2D::LinePlot2D(int w, int h, std::string plot_name)
    : Plot(w, h, fullscreen_quad_vert_spirv, grid2d_frag_spirv),
      grid_program(fullscreen_quad_vert_spirv, grid2d_frag_spirv),
      line_program(line_plot_vert_spirv, line_plot_frag_spirv)
{
    name = plot_name;
    x_min_factor = -10.0f;
    x_max_factor =  10.0f;

    init_default_series();
}

LinePlot2D::~LinePlot2D() {}

void LinePlot2D::init_default_series() {
    // 1. Sine Wave
    add_function("Sine Wave", [](float x) {
        return 2.0f * std::sin(0.8f * x);
    }, -15.0f, 15.0f, 1200, glm::vec4(0.20f, 0.80f, 1.00f, 1.0f), 3.0f);

    // 2. Cosine Wave
    add_function("Cosine Wave", [](float x) {
        return 1.5f * std::cos(1.2f * x);
    }, -15.0f, 15.0f, 1200, glm::vec4(1.00f, 0.55f, 0.15f, 1.0f), 2.5f);

    // 3. Damped Oscillation
    add_function("Damped Wave", [](float x) {
        return 3.0f * std::exp(-0.15f * std::abs(x)) * std::cos(2.0f * x);
    }, -15.0f, 15.0f, 1200, glm::vec4(0.25f, 0.95f, 0.55f, 1.0f), 2.5f);
}

void LinePlot2D::add_series(const std::string& series_name, const std::vector<glm::vec2>& points, const glm::vec4& color, float line_width) {
    series_list.emplace_back(series_name, points, color, line_width);
}

void LinePlot2D::add_function(const std::string& series_name, std::function<float(float)> func, float x_min, float x_max, int num_samples, const glm::vec4& color, float line_width) {
    int samples = std::max(num_samples, 2);
    std::vector<glm::vec2> pts;
    pts.reserve(samples);

    float step = (x_max - x_min) / static_cast<float>(samples - 1);
    for (int i = 0; i < samples; ++i) {
        float px = x_min + i * step;
        float py = func(px);
        pts.emplace_back(px, py);
    }

    add_series(series_name, pts, color, line_width);
}

void LinePlot2D::toggle_series(size_t index) {
    if (index < series_list.size()) {
        series_list[index].visible = !series_list[index].visible;
    }
}

void LinePlot2D::update_series(size_t index, const std::vector<glm::vec2>& points) {
    if (index < series_list.size()) {
        series_list[index].update_points(points);
    }
}

void LinePlot2D::render() {
    if (width <= 0 || height <= 0) return;

    // Step 1: Draw Background Canvas & Coordinate Grid
    glDisable(GL_BLEND);
    glUseProgram(grid_program);
    upload_common_uniforms();
    pre_draw();

    // Step 2: Render Vertex Lines via LineMesh RAII objects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    glUseProgram(line_program);
    upload_common_uniforms();

    for (auto& series : series_list) {
        if (!series.visible) continue;
        series.mesh.draw(series.line_width, series.color);
    }

    glUseProgram(0);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}

void LinePlot2D::handle_input(GLFWwindow* window) {
    Plot::handle_input(window);

    // Toggle individual vertex line series with number keys 1..5
    static bool key_states[5] = {false, false, false, false, false};
    for (int key = GLFW_KEY_1; key <= GLFW_KEY_5; ++key) {
        size_t idx = static_cast<size_t>(key - GLFW_KEY_1);
        int state = glfwGetKey(window, key);
        if (state == GLFW_PRESS) {
            if (!key_states[idx]) {
                toggle_series(idx);
                key_states[idx] = true;
            }
        } else if (state == GLFW_RELEASE) {
            key_states[idx] = false;
        }
    }
}
