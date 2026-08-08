
#include "scene/mandelbrot.h"
#include "scene/julia.h"
#include "scene/generic.h"
#include "ogl/app.h"

int main()
{

    constexpr int width  = 3840; 
    constexpr int height = 2160;
    //std::println("I initialized mandelbrot");
    App app(width,height);
    if (app.init() != 0) return 1;
    auto mandelbrot = std::make_shared<MandelbrotScene>(width, height);
    auto julia = std::make_shared<JuliaScene>(width, height);
    auto generic = std::make_shared<GenericScene>(width, height);
    app.add_scene(generic);
    app.mainloop();
    return 0;
}

