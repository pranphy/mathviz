#include <print>
#include <format>

#include "scene/scene.h"
#include "writer/ppm.h"

Scene::Scene(int w, int h):
    buffer(h*w*3),
    name{"generic_scene"}
{}

void Scene::set_resolution(int width, int height){
    this->width = width;
    this->height = height;
    buffer.resize(width*height*3);
    std::println("Scene={} size changed to ({},{})",name,width,height);
}

void Scene::save_scene(){
    std::println("The current size is {},{}",width,height);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    write_ppm(std::format("{}_current_scene.ppm",name), width, height, buffer);
}

void Scene::step_right(){
    float step = 0.01f * scale;
    x += step;
}

void Scene::step_left(){
    float step = 0.01f * scale;
    x -= step;
}

void Scene::step_up(){
    float step = 0.01f * scale;
    y += step;
}

void Scene::step_down(){
    float step = 0.01f * scale;
    y -= step;
}

void Scene::zoom_in(){
    float step = 0.01f * scale;
    scale -= step;
}

void Scene::zoom_out(){
    float step = 0.01f * scale;
    scale += step;
}

void Scene::iterate_up(){
    max_iterations++;
}

void Scene::iterate_down(){
    if (max_iterations > 0) max_iterations--;
}

Scene::~Scene(){}

void Scene::render() const {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(render_data.shader_program);

    glUniform1f(0, static_cast<float>(width));
    glUniform1f(1, static_cast<float>(height));

    glUniform2f(2,
        x_min_factor * scale + x,
        x_max_factor * scale + x
    );

    glUniform2f(3,
        y_min_factor * scale + y,
        y_max_factor * scale + y
    );

    glUniform1ui(4, max_iterations);

    // Call subclass hook for additional uniforms (if any)
    setup_uniforms();

    glBindVertexArray(render_data.vertex_array_object);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
}

