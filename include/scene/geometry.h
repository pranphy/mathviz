#pragma once

#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "scene/scene.h"

struct GeometryScene : public Scene
{
    GeometryScene(int w, int h);
    virtual bool run(GLFWwindow* window, int mode) override;
    virtual void mouse_drag(float dx, float dy) override;
    virtual ~GeometryScene();
};
