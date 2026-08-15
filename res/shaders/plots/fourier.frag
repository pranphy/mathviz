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

    uint terms = max(max_iterations, 1u);
    vec3 col = (length(u_color) > 0.01) ? u_color : vec3(1.00, 0.90, 0.25);

    // Fourier Series of Square Wave
    float dy_sq = 0.0;
    float y_sq = fourier_square_wave(pos.x, terms, dy_sq);
    vec4 layer1 = draw_explicit_curve(pos.y, y_sq, dy_sq, col, 2.5, px, glow);

    pixel_color = layer1;
}
