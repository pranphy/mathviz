#include <print>
#include <stdexcept>
#include "ogl/app.h"

void handle_input(GLFWwindow* window, std::shared_ptr<Scene> s, bool& running)
{
    glfwPollEvents();
    if (glfwWindowShouldClose(window)) running = false;

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) s->step_up();
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) s->step_down();
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) s->step_left();
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) s->step_right();

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) s->zoom_out();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) s->zoom_in();

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) s->iterate_up();
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) s->iterate_down();
    //std::println(" x = {}, y = {}, scale = {}, max_iterations = {}", m.x, m.y, m.scale, m.max_iterations);
}

App::App(int w, int h):
    window_width(w), window_height(h),
     window{nullptr}, mode(Mode::TUI)
{
}

int App::init(){
    if (!glfwInit()) {
        std::print("GLFW initialization error\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if(mode == Mode::TUI){
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // hide GUI
    }
     window = glfwCreateWindow(window_width, window_height, "MandelbrotGL", nullptr, nullptr);
    if (!window) {
        std::print("GLFW window creation error\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    return 0;
}

void App::add_scene(std::shared_ptr<Scene> s){
    scene = s;
}


void App::mainloop(){

    try {
        bool running = true;
        while (running) {
            handle_input(window, scene, running);
            running = scene->run(window,0);
            //running = false;
            //callback();
            //mandelbrot.render();
            //glfwSwapBuffers(window);
        }
    }
    catch (const std::exception& ex) {
        std::print("{}\n", ex.what());
    }
}

App::~App(){
    scene.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

