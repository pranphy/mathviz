#include <print>
#include <format>

#include "scene/scene.h"
#include "writer/ppm.h"

Scene::Scene(int w, int h, std::span<const uint32_t> vs_spirv, std::span<const uint32_t> fs_spirv):
    width(w),
    height(h),
    buffer(h * w * 3),
    name{"generic_scene"},
    shader_program{vs_spirv, fs_spirv}
{
    create_rectangle_vao(rectangle_buffer, rectangle_vao);
}


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

void Scene::mouse_drag(float dx, float dy){
    x -= static_cast<float>((dx / width)  * scale * 2.0);
    y += static_cast<float>((dy / height) * scale * 2.0);
}

Scene::~Scene(){}

void Scene::pre_draw(){
    glBindVertexArray(rectangle_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Scene::post_draw() {}

void Scene::render() {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

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
    glUniform1f(6, static_cast<float>(glfwGetTime()));

    // Call subclass hook for additional uniforms (if any)
    setup_uniforms();

    // Call custom subclass states hook
    pre_draw();


    // Call custom subclass cleanup hook
    post_draw();

    glUseProgram(0);
}

