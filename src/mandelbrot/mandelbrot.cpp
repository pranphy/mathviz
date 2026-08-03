#include <filesystem>
#include <glm/glm.hpp>


#include "mandelbrot/mandelbrot.h"



MandelbrotScene::MandelbrotScene(int w, int h):
      buffer(h*w*3),
      rectangle_buffer{},
      shader_program{
        read_file(std::filesystem::path{"res/mandelbrot/mandelbrot_shader.vs"}),
        read_file(std::filesystem::path{"res/mandelbrot/mandelbrot_shader.fs"})
      }
{
    width = w;
    height = h;
    scale = 1.0f;
    x = 0.0f;
    y = 0.2f;
    max_iterations = 30;

    create_rectangle_vao(rectangle_buffer, rectangle_vao);
    render_data = RenderData{shader_program, rectangle_vao};

}

void MandelbrotScene::set_param(MandelbrotParam p){
    std::tie(x,y,scale,max_iterations) = p;
}

void MandelbrotScene::render() const 
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(render_data.shader_program);

    glUniform1f(0, width);
    glUniform1f(1, height);

    glUniform2f(2,
        -2.f * scale + x,
         1.f * scale + x
    );

    glUniform2f(3,
        -1.f * scale + y,
         1.f * scale + y
    );

    glUniform1ui(4, max_iterations);

    glBindVertexArray(render_data.vertex_array_object);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glUseProgram(0);
}


void MandelbrotScene::move_along_path(MandelbrotScene m1, MandelbrotScene m2, int frames,VideoWriter& wr,GLFWwindow* window) {
        // Animation parameters
        const int total_frames = frames;
        glm::vec2 start_center(m1.x, m1.y);
        glm::vec2 end_center(m2.x, m2.y);
        float start_zoom = m1.scale;
        float end_zoom   = m2.scale;

        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        for (int frame = 0; frame < total_frames; ++frame) {
            float t = frame / float(total_frames - 1);
            glm::vec2 center = glm::mix(start_center, end_center, t);
            scale = std::exp(glm::mix(std::log(start_zoom), std::log(end_zoom), t));
            x = center.x;
            y = center.y;
            render();
            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
            wr.push_frame(buffer.data());
            //std::cout.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
            glfwSwapBuffers(window);
        }

    }

void MandelbrotScene::animate_to(MandelbrotParam m1, int frames,VideoWriter& wr,GLFWwindow* window) {
    // Animation parameters
    auto [xn, yn, scalen, max_iterationsn] = m1;
    const int total_frames = frames;
    glm::vec2 start_center(x, y);
    glm::vec2 end_center(xn, yn);
    float start_zoom = scale;
    float end_zoom   = scalen;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);

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

bool MandelbrotScene::run(GLFWwindow* window,int /* unused */){
    //std::println("I am running scene");

    VideoWriter wr("./mandelbert-0003.mp4",width,height,30);
    std::vector<MandelbrotParam> keyframes {
        { -1.7252895f,0.03053458f, 0.08974f, 30},
        {-1.8309544f, 0.000772018f, 0.0017459377f,  39},
        {-1.83285550f,2.7735707e-5f, 7.949502e-5f,   47},
    };
    for(auto keyframe : keyframes){
        animate_to(keyframe,300,wr,window);
    }
    //render();
    glfwSwapBuffers(window);
    return false;
}

MandelbrotScene::~MandelbrotScene(){}
