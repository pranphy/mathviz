// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
#extension GL_GOOGLE_include_directive : require

#include "common_uniforms.glsl"
#include "math_functions.glsl"

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

    // Adaptive Coordinate Grid & Axes
    float view_span = area_w.y - area_w.x;
    float log_val = log(max(view_span, 1e-6) / 8.0) * (1.0 / LN10);
    float base_step = pow(10.0, floor(log_val));
    float rel = view_span / (8.0 * base_step);

    float major_step = base_step;
    if (rel > 5.0) major_step *= 5.0;
    else if (rel > 2.0) major_step *= 2.0;
    float minor_step = major_step * 0.2;

    // Dark sleek canvas
    vec3 color = vec3(0.06, 0.07, 0.10);

    bool show_grid = (u_flags & 1u) != 0u;
    bool show_axes = (u_flags & 2u) != 0u;
    bool show_cur  = (u_flags & 4u) != 0u;

    if (show_grid) {
        vec2 minor_g = abs(fract(pos / minor_step + 0.5) - 0.5) * minor_step;
        float d_minor = min(minor_g.x, minor_g.y);
        color = mix(color, vec3(0.12, 0.15, 0.20), (1.0 - smoothstep(0.0, px * 1.5, d_minor)) * 0.5);

        vec2 major_g = abs(fract(pos / major_step + 0.5) - 0.5) * major_step;
        float d_major = min(major_g.x, major_g.y);
        color = mix(color, vec3(0.20, 0.25, 0.33), (1.0 - smoothstep(0.0, px * 2.0, d_major)) * 0.75);
    }

    if (show_axes) {
        float d_axis = min(abs(pos.y), abs(pos.x));
        color = mix(color, vec3(0.55, 0.62, 0.75), (1.0 - smoothstep(0.0, px * 2.2, d_axis)) * 0.9);

        float d_orig = length(pos);
        color = mix(color, vec3(0.9, 0.95, 1.0), (1.0 - smoothstep(px * 3.0, px * 4.5, d_orig)) * 0.9);
    }

    if (show_cur) {
        float d_cur = length(pos - u_cursor);
        float cur_circle = 1.0 - smoothstep(px * 4.0, px * 6.0, abs(d_cur - px * 7.0));
        color = mix(color, vec3(1.0, 1.0, 1.0), cur_circle * 0.9);
        float cur_dot = 1.0 - smoothstep(0.0, px * 3.0, d_cur);
        color = mix(color, vec3(1.0, 0.9, 0.25), cur_dot);
    }

    pixel_color = vec4(color, 1.0);
}
