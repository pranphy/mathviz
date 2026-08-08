// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]
// date   : 2026-08-05

#version 460 core
#extension GL_GOOGLE_include_directive : require

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;
layout(location = 5) uniform vec2 julia_c;

layout(location = 0) out vec4 pixel_color;

#include "colormap.glsl"

void main()
{
    // Z is mapped from the screen coordinates
    vec2 Z = vec2(gl_FragCoord.x * (area_w.y - area_w.x) / rect_width  + area_w.x,
                  gl_FragCoord.y * (area_h.y - area_h.x) / rect_height + area_h.x);
    vec2 C = julia_c;
    uint iteration = 0;
    float z_sq_dist = 0.0;

    while (iteration < max_iterations)
    {
        float x = Z.x * Z.x - Z.y * Z.y + C.x;
        float y = 2.0 * Z.x * Z.y        + C.y;

        z_sq_dist = x * x + y * y;
        if (z_sq_dist > 4.0)
            break;

        Z.x = x;
        Z.y = y;

        ++iteration;
    }

    pixel_color = vec4(get_smooth_color(iteration, z_sq_dist, max_iterations), 1.0);
}
