#pragma once

#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "scene/scene.h"

struct AtomScene : public Scene
{
    std::vector<glm::vec4> particles; // Nucleus at 0 and 1, electrons in remaining indices
    VertexArray particles_vao;
    Buffer particles_vbo;
    const int num_electrons;

    float r_p;
    float r_n;
    float r_e;

    AtomScene(int w, int h);
    virtual void pre_draw() override;
    virtual void post_draw() override;
    virtual void setup_uniforms() override;
    virtual bool run(GLFWwindow* window, int mode) override;
    void update_particles(unsigned, unsigned);
    void sample_electrons();
    virtual ~AtomScene();

};
