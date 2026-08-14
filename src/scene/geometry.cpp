#include "scene/geometry.h"
#include "geometry_vert.h"
#include "geometry_frag.h"

GeometryScene::GeometryScene(int w, int h) :
    Scene(w, h, geometry_vert_spirv, geometry_frag_spirv)
{
    max_iterations = 80; // Reused as raymarching max steps
    is_animated = true;
    name = "geometry_scene";

}

void GeometryScene::mouse_drag(float dx, float dy) {
    x += static_cast<float>((dx / width) * 4.0);
    y -= static_cast<float>((dy / height) * 4.0);
}

bool GeometryScene::run(GLFWwindow* window, int mode) {
    (void)window;
    if (mode == 0) { // GUI mode
        render();
        return true;
    }
    return false;
}

GeometryScene::~GeometryScene() {}
