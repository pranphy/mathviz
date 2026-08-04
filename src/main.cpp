
#include "scene/mandelbrot.h"
#include "scene/julia.h"
#include "ogl/app.h"

int main()
{

    constexpr int width  = 1920; 
    constexpr int height = 1080;
    //std::println("I initialized mandelbrot");
    App app(width,height);
    if (app.init() != 0) return 1;
    auto mandelbrot = std::make_shared<MandelbrotScene>(width, height);
    auto julia = std::make_shared<JuliaScene>(width, height);
    app.add_scene(julia);
    app.mainloop();
    return 0;
}

