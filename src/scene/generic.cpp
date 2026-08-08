#include <print>
#include <glm/glm.hpp>

#include "scene/generic.h"
#include "julia_vert.h"
#include "generic_frag.h"
#include "standing_wave_frag.h"


GenericScene::GenericScene(int w, int h):
      Scene(w, h, julia_vert_spirv, standing_wave_frag_spirv)
{
    width = w;
    height = h;
    scale = 1.0f;
    x = 0.0f;
    y = 0.2f;
    max_iterations = 30;
    name = "generic_scene";

    x_min_factor = -2.f;
    x_max_factor =  1.f;
    y_min_factor = -1.f;
    y_max_factor =  1.f;
}
void GenericScene::set_resolution(int w, int h){
    width  = w;
    height = h;
    std::println("Mandelbert: The size changed to ({},{})",width,height);
    buffer.resize(w*h*3);
}

void GenericScene::setup_uniforms() const {
    glUniform1f(6, static_cast<float>(glfwGetTime()));
}

void GenericScene::set_param(GenericParam p){
    std::tie(x,y,scale,max_iterations) = p;
}


bool GenericScene::save_video(GLFWwindow* window,int /* unused */){
    // Detect whether the window is visible (Mode::GUI vs Mode::TUI)

    // Initialize VideoWriter with the target render resolution
    int fps = 30;
    VideoWriter wr("./generic-0004.mp4", width, height, fps);
    int time = 10; // seconds
    int total_frames = fps * time;
    for(int i = 0; i < total_frames; i++){
        render();
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        wr.push_frame(buffer.data());
        glfwSwapBuffers(window);
    }

    return false;
}

bool GenericScene::run(GLFWwindow* window,int mode){
    // Detect whether the window is visible (Mode::GUI vs Mode::TUI)
    if(mode == 0){ // GUI is 0 and TUI is 1
        render();
        return true;
    } else {
        return save_video(window,0);
    }

    return false;
}

GenericScene::~GenericScene(){}
