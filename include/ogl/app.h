#pragma once

#include <memory>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "scene/scene.h"


void handle_input(GLFWwindow* window, Scene& s, bool& running);

enum class Mode {
    GUI = 0,
    TUI = 1
};

struct App {
    int window_width;
    int window_height;

    std::shared_ptr<Scene> scene;
    GLFWwindow* window;
    Mode mode;
    void add_scene(std::shared_ptr<Scene>);

    GLuint fbo = 0;
    GLuint rbo = 0;

    App(int  w, int h);
    int init(); 
    void mainloop();
    ~App();
};

