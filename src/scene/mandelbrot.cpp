#include <vector>
#include <glm/glm.hpp>

#include "scene/mandelbrot.h"
#include "fullscreen_quad_vert.h"
#include "mandelbrot_frag.h"

MandelbrotScene::MandelbrotScene(int w, int h):
      Scene(w, h, fullscreen_quad_vert_spirv, mandelbrot_frag_spirv)
{
    width = w;
    height = h;
    max_iterations = 30;
    name = "mandelbrot_scene";

    x_min_factor = -2.f;
    x_max_factor =  1.f;
}

void MandelbrotScene::set_param(MandelbrotParam p){
    std::tie(x,y,scale,max_iterations) = p;
}

void MandelbrotScene::animate_to(MandelbrotParam m1, int frames,VideoWriter& wr,GLFWwindow* window) {
    // Animation parameters
    auto [xn, yn, scalen, max_iterationsn] = m1;
    const int total_frames = frames;
    glm::vec2 start_center(x, y);
    glm::vec2 end_center(xn, yn);
    float start_zoom = scale;
    float end_zoom   = scalen;


    for (int frame = 0; frame < total_frames; ++frame) {
        float t = frame / float(total_frames - 1);

        glm::vec2 center = glm::mix(start_center, end_center, t);
        scale = std::exp(glm::mix(std::log(start_zoom), std::log(end_zoom), t));
        //scale = end_zoom;
        x = center.x;
        y = center.y;
        render();
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
        wr.push_frame(buffer.data());
        glfwSwapBuffers(window);
    }

}

bool MandelbrotScene::save_video(GLFWwindow* window,int /* unused */){
    // Detect whether the window is visible (Mode::GUI vs Mode::TUI)

    // Initialize VideoWriter with the target render resolution
    VideoWriter wr("./mandelbrot-0004.mp4", width, height, 30);

    // Resize reading buffer
    //buffer.resize(render_width * render_height * 3);

    std::vector<MandelbrotParam> keyframes {
        {-1.7252895f, 0.03053458f, 0.08974f, 30},
        {-1.8309544f, 0.000772018f, 0.0017459377f,  39},
        {-1.83285550f,2.7735707e-5f, 7.949502e-5f,   47},
    };
    for(auto keyframe : keyframes){
        animate_to(keyframe,100,wr,window);
    }

    return false;
}

bool MandelbrotScene::run(GLFWwindow* window,int mode){
    // Detect whether the window is visible (Mode::GUI vs Mode::TUI)
    if(mode == 0){ // GUI is 0 and TUI is 1
        render();
        return true;
    } else {
        return save_video(window,0);
    }

    return false;
}

MandelbrotScene::~MandelbrotScene(){}
