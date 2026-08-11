
#include "scene/mandelbrot.h"
#include "scene/julia.h"
#include "scene/generic.h"
#include "scene/geometry.h"
#include "scene/atom.h"
#include "ogl/app.h"
#include "ogl/tui.h"

int gui(){
    constexpr int width  = 3840;
    constexpr int height = 2160;
    App app(width,height);
    if (app.init() != 0) return 1;
    auto mandelbrot = std::make_shared<MandelbrotScene>(width, height);
    auto julia = std::make_shared<JuliaScene>(width, height);
    auto generic = std::make_shared<GenericScene>(width, height);
    auto geometry = std::make_shared<GeometryScene>(width, height);
    auto atom = std::make_shared<AtomScene>(width, height);
    app.add_scene(mandelbrot);
    app.mainloop();
    return 0;
}

int tui(){
    do_things();
    return 0;
}

int main()
{
    return gui();
    return 0;
}

