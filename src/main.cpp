#include "scene/plot2d.h"
#include "scene/plot3d.h"
#include "scene/line_plot2d.h"
#include "ogl/app.h"
#include "ogl/tui.h"

int gui(){
    constexpr int width  = 3840;
    constexpr int height = 2160;
    App app(width, height);
    if (app.init() != 0) return 1;

    auto line_plot2d = std::make_shared<LinePlot2D>(width, height);
    auto plot2d      = std::make_shared<Plot2D>(width, height);
    auto plot3d      = std::make_shared<Plot3D>(width, height);

    app.add_scene(line_plot2d);
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

