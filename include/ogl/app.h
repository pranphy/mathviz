#pragma once

#include <memory>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "scene/scene.h"


void handle_input(GLFWwindow* window, Scene& s, bool& running);

enum class Mode {
    GUI,
    TUI
};

struct App {
    const int window_width;
    const int window_height;

    std::shared_ptr<Scene> scene;
    GLFWwindow* window;
    Mode mode;
    void add_scene(std::shared_ptr<Scene>);

    App(int  w, int h);
    int init(); 
    void mainloop();
    ~App();
};

