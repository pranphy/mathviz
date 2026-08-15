#pragma once

#include <span>
#include <string>
#include "scene/plot.h"
#include "plot3d_frag.h"

// 3D Surface & Heightfield Plotter derived from Plot
class Plot3D : public Plot
{
public:
    float cam_yaw;   // 3D camera yaw (horizontal orbit)
    float cam_pitch; // 3D camera pitch (vertical elevation)
    float cam_dist;  // 3D camera distance from origin

    Plot3D(int w, int h, std::span<const uint32_t> fs_spirv = plot3d_frag_spirv, std::string plot_name = "plot3d");
    virtual ~Plot3D() override;

    virtual void render() override;
    virtual void mouse_drag(float dx, float dy) override;
    virtual void reset_view() override;
};
