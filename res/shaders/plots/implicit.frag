// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
#extension GL_GOOGLE_include_directive : require

#include "../common_uniforms.glsl"
#include "../math_functions.glsl"

layout(location = 0) out vec4 pixel_color;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(rect_width, rect_height);
    vec2 pos = vec2(
        mix(area_w.x, area_w.y, uv.x),
        mix(area_h.x, area_h.y, uv.y)
    );

    vec2 pixel_size = vec2(
        (area_w.y - area_w.x) / rect_width,
        (area_h.y - area_h.x) / rect_height
    );
    float px = pixel_size.y;
    float glow = ((u_flags & 8u) != 0u) ? 0.95 : 0.0;

    // Heart Curve: (x^2 + y^2 - 1)^3 - x^2 * y^3 = 0
    float top = pos.x * pos.x + pos.y * pos.y - 1.0;
    float f_heart = top * top * top - pos.x * pos.x * pos.y * pos.y * pos.y;
    vec2 grad_heart = vec2(
        6.0 * pos.x * top * top - 2.0 * pos.x * pow(pos.y, 3.0),
        6.0 * pos.y * top * top - 3.0 * pos.x * pos.x * pos.y * pos.y
    );

    vec3 col = (length(u_color) > 0.01) ? u_color : vec3(0.95, 0.30, 0.85);
    vec4 layer1 = draw_implicit_curve(f_heart, grad_heart, col, 2.5, px, glow);

    pixel_color = layer1;
}
