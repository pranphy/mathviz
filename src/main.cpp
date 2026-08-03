
#include "mandelbrot/mandelbrot.h"
#include "ogl/app.h"

int main()
{

    constexpr int width  = 3840; 
    constexpr int height = 2160;
    //std::println("I initialized mandelbrot");
    App app(width,height);
    if (app.init() != 0) return 1;
    auto mandelbrot = std::make_shared<MandelbrotScene>(width, height);
    app.add_scene(mandelbrot);
    app.mainloop();
    return 0;
}

