#include <print>
#include <stdexcept>
#include "ogl/app.h"

void handle_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Only react when a key is pressed or held down (repeat)
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }
    // Retrieve scene from window user pointer
    auto s = static_cast<std::shared_ptr<Scene>*>(glfwGetWindowUserPointer(window));
    if (!s || !*s) return;
    switch (key) {
        case GLFW_KEY_K:      (*s)->step_up(); break;
        case GLFW_KEY_J:      (*s)->step_down(); break;
        case GLFW_KEY_H:      (*s)->step_left(); break;
        case GLFW_KEY_L:      (*s)->step_right(); break;
        case GLFW_KEY_S:      (*s)->zoom_out(); break;
        case GLFW_KEY_D:      (*s)->zoom_in(); break;
        case GLFW_KEY_F:      (*s)->iterate_up(); break;
        case GLFW_KEY_G:      (*s)->iterate_down(); break;
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
}

void handle_resize(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);

    // 2. Retrieve your Scene pointer from the window user pointer
    auto s = static_cast<std::shared_ptr<Scene>*>(glfwGetWindowUserPointer(window));
    if (s && *s) {
        // Update the scene with the new width and height
        (*s)->set_resolution(width, height);
    }
}

App::App(int w, int h):
    window_width(w), window_height(h),
    window{nullptr}, mode(Mode::GUI),
    fbo(0), rbo(0)
{
}

int App::init() {
    if (!glfwInit()) {
        std::print("GLFW initialization error\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (mode == Mode::TUI) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide window for TUI mode
    }
    window = glfwCreateWindow(window_width, window_height, "MandelbrotGL", nullptr, nullptr);
    if (!window) {
        std::print("GLFW window creation error\n");
        glfwTerminate();
        return 1;
    }
    glfwSetFramebufferSizeCallback(window, handle_resize);
    glfwSetKeyCallback(window, handle_key);
    glfwMakeContextCurrent(window);
    // 1. MUST load GLAD first
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    // 2. NOW you can call OpenGL functions (FBO / RBO setup)
    if (mode == Mode::TUI) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, window_width, window_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Offscreen framebuffer is not complete");
        }
        //glfwSwapInterval(0);
    } else {
        //glfwSwapInterval(1);
        glfwGetFramebufferSize(window, &window_width, &window_height);
    }
    return 0;
}


void App::add_scene(std::shared_ptr<Scene> s){
    scene = s;
    glfwSetWindowUserPointer(window, &scene);
}


void App::mainloop(){
    //int i = 0;

    try {
        bool running = true;
        int runmode = static_cast<int>(mode);
        while (running && !glfwWindowShouldClose(window)) {
            running = scene->run(window,runmode);
            glfwSwapBuffers(window);
            glfwWaitEvents(); // Blocks the CPU completely until an OS event occurs!
            //std::println("{}. Updated ",++i);
        }
    }
    catch (const std::exception& ex) {
        std::print("{}\n", ex.what());
    }
}

App::~App(){
    if (mode == Mode::TUI){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteRenderbuffers(1, &rbo);
        glDeleteFramebuffers(1, &fbo);
    }
    scene.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

