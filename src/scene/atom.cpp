#include <cmath>
#include <glad/gl.h>
#include <glm/glm.hpp>

#include "atom_frag.h"
#include "atom_vert.h"
#include "scene/atom.h"
#include "utils/math.h"

AtomScene::AtomScene(int w, int h)
    : Scene(w, h, atom_vert_spirv, atom_frag_spirv), num_electrons{2'000'000},
    r_p(0.12f), r_n(0.12f), r_e(0.005f)
{
    max_iterations = 80;
    is_animated = true;
    name = "atom_scene";

    particles.resize(10 + num_electrons);

    const float phi = pi * (3.0f - std::sqrt(5.0f)); // Golden angle = 2pi / phi
    const float shell_radius = 0.85f;

    for (int i = 0; i < num_electrons; ++i) {
        float y_val = 1.0f - (i / (num_electrons - 1.0f)) * 2.0f; // range [-1, 1]
        float r_val = std::sqrt(1.0f - y_val * y_val); // radius at y coordinate
        float theta = phi * i;

        float x_val = std::cos(theta) * r_val;
        float z_val = std::sin(theta) * r_val;

        glm::vec3 pos = glm::vec3(x_val, y_val, z_val) * shell_radius;
        float color_attr = (y_val + 1.0f) * 0.5f; // Map height to color attribute

        particles[2 + i] = glm::vec4(pos, color_attr);
        //std::println("{}, {}, {}", pos.x, pos.y, pos.z);
    }

    // Call standard library helper to create vbo and configure vao attributes
    create_points_vao(particles_vbo, particles_vao, particles);

    sample_electrons();

    // Bind custom VAO and drawing parameters to the base Scene class
    // draw_mode = GL_POINTS;
    // vertex_count = static_cast<GLsizei>(particles.size());
}

void AtomScene::pre_draw() {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBindVertexArray(particles_vao);
    glDrawArrays(
        GL_POINTS, 0,
        static_cast<GLsizei>(particles.size())); // all hell breaks loose here.
    glBindVertexArray(0);
}

void AtomScene::post_draw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

float coef(unsigned n, unsigned l, float a) {
    float pr = 2.0 / (n * a);
    float fac1 = pow(pr, 3);
    float fac2 = fct(n - l - 1) / (2 * n * pow(fct(n + l), 3));
    return std::sqrt(fac1 * fac2);
}

float coefsq(unsigned n, unsigned l, float a) {
    float pr = 2.0 / (n * a);
    float fac1 = pow(pr, 3);
    float fac2 = fct(n - l - 1) / (2 * n * pow(fct(n + l), 3));
    return fac1 * fac2;
}

float Rnl(unsigned n, unsigned l, float r, float a) {
    float rho = 2 * r / (n * a);
    float fac3 = std::exp(-rho / 2.0) * std::pow(rho, l) * std::assoc_laguerref(n - l - 1, 2 * l + 1, rho);
    float coef =  std::sqrt(fct(n-l-1)/(2.0*n*fct(n+l)));
    return coef*fac3;
}

float Rnlsq(unsigned n, unsigned l, float r, float a) {
    float rho = 2 * r / (n * a);
    float fac3 = std::exp(-rho) * std::pow(rho, 2*l) * std::pow(std::assoc_laguerref(n - l - 1, 2 * l + 1, rho),2);
    float coef =  fct(n-l-1)/(2.0*n*fct(n+l));
    return coef*fac3;
}

float psisqnlmsq(unsigned l, unsigned m, float theta) {
    float fac2 = pow(std::sph_legendref(l, m, theta), 2);
    return fac2 * std::sin(theta);
}

void AtomScene::sample_electrons() {
    int num_bins = 1000;
    constexpr float a = 0.18;
    constexpr unsigned n = 4;
    constexpr unsigned l = 3;
    constexpr unsigned m = 0;
    BinnedInverseSampler r_sampler([&](float r) { return r * r * Rnlsq(n, l, r, a); }, 0.0f, 20.0 * a, num_bins);
    BinnedInverseSampler theta_sampler( [](float theta) { return psisqnlmsq(l, m, theta); }, 0.0f, std::numbers::pi, num_bins);

    auto phid = std::uniform_real_distribution<float>(0.0f, 2.0f * std::numbers::pi);

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < num_electrons; ++i) {
        float r = r_sampler.sample(gen)/(6*a);
        float theta = theta_sampler.sample(gen);
        float phi = phid(gen);

        float x = r * std::sin(theta) * std::cos(phi);
        float y = r * std::sin(theta) * std::sin(phi);
        float z = r * cos(theta);

        glm::vec3 pos = glm::vec3(x, y, z);
        float color_attr = std::min(r * 0.5f,1.0f);
        particles[2 + i] = glm::vec4(pos, color_attr);

    }
    update_particles(2, num_electrons);
}

void AtomScene::update_particles(unsigned start, unsigned num) {
    glBindBuffer(GL_ARRAY_BUFFER, particles_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(glm::vec4), num * sizeof(glm::vec4), particles.data() + start);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AtomScene::setup_uniforms() {
    float t = static_cast<float>(glfwGetTime()) * 2.0f;
    const float pf = 0.00 * r_p;
    glm::vec3 proton_pos(pf * std::cos(t), pf * std::sin(t * 0.7f), pf * std::sin(t));
    glm::vec3 neutron_pos(-proton_pos.x, -proton_pos.y, -proton_pos.z);

    //particles[0] = glm::vec4(proton_pos, -1.0f);

    update_particles(0, 1);

    glUniform1f(10, r_p);
    glUniform1f(11, r_n);
    glUniform1f(12, r_e);
}

bool AtomScene::run(GLFWwindow *window, int mode) {
    if (mode == 0) { // GUI mode
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            r_p += 0.0015f;
            r_n += 0.0015f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            r_p = std::max(r_p - 0.0015f, 0.01f);
            r_n = std::max(r_n - 0.0015f, 0.01f);
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            r_e += 0.0002f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            r_e = std::max(r_e - 0.0002f, 0.001f);
        }

        render();
        return true;
    }
    return false;
}

AtomScene::~AtomScene() {}
