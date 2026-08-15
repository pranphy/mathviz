// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
#extension GL_GOOGLE_include_directive : require

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations; // used as harmonic count / series depth
layout(location = 6) uniform float u_time;
layout(location = 8) uniform uint param;          // preset / plot mode selector

layout(location = 10) uniform vec4 u_params;      // custom user parameters (a, b, c, d)
layout(location = 11) uniform vec2 u_cursor;      // cursor in world coordinates
layout(location = 12) uniform uint u_flags;       // bit 0: grid, bit 1: axes, bit 2: cursor/tangent, bit 3: neon glow

layout(location = 0) out vec4 pixel_color;

#include "math_functions.glsl"

vec3 blend_over(vec3 bg, vec4 layer) {
    return mix(bg, layer.rgb, layer.a);
}

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
    float dx = pixel_size.x * 0.5;
    float dy = pixel_size.y * 0.5;

    // --- Adaptive Coordinate Grid & Axes ---
    float view_span = area_w.y - area_w.x;
    float log_val = log(max(view_span, 1e-6) / 8.0) * (1.0 / LN10);
    float base_step = pow(10.0, floor(log_val));
    float rel = view_span / (8.0 * base_step);

    float major_step = base_step;
    if (rel > 5.0) {
        major_step *= 5.0;
    } else if (rel > 2.0) {
        major_step *= 2.0;
    }
    float minor_step = major_step * 0.2;

    vec3 color = vec3(0.06, 0.07, 0.10);

    bool show_grid = (u_flags & 1u) != 0u;
    bool show_axes = (u_flags & 2u) != 0u;
    float glow     = ((u_flags & 8u) != 0u) ? 0.95 : 0.0;

    if (show_grid) {
        vec2 minor_g = abs(fract(pos / minor_step + 0.5) - 0.5) * minor_step;
        float d_minor = min(minor_g.x, minor_g.y);
        float minor_alpha = 1.0 - smoothstep(0.0, px * 1.5, d_minor);
        color = mix(color, vec3(0.12, 0.15, 0.20), minor_alpha * 0.5);

        vec2 major_g = abs(fract(pos / major_step + 0.5) - 0.5) * major_step;
        float d_major = min(major_g.x, major_g.y);
        float major_alpha = 1.0 - smoothstep(0.0, px * 2.0, d_major);
        color = mix(color, vec3(0.20, 0.25, 0.33), major_alpha * 0.75);
    }

    if (show_axes) {
        float d_axis = min(abs(pos.y), abs(pos.x));
        float axis_alpha = 1.0 - smoothstep(0.0, px * 2.2, d_axis);
        color = mix(color, vec3(0.55, 0.62, 0.75), axis_alpha * 0.9);

        float d_orig = length(pos);
        float orig_alpha = 1.0 - smoothstep(px * 3.0, px * 4.5, d_orig);
        color = mix(color, vec3(0.9, 0.95, 1.0), orig_alpha * 0.9);
    }

    float t = u_time;
    float a = u_params.x;
    float b = u_params.y;
    float c = u_params.z;
    float d = u_params.w;
    uint terms = max(max_iterations, 1u);

    // Color Palette
    vec3 c_cyan    = vec3(0.20, 0.80, 1.00);
    vec3 c_orange  = vec3(1.00, 0.55, 0.15);
    vec3 c_green   = vec3(0.25, 0.95, 0.55);
    vec3 c_magenta = vec3(0.95, 0.30, 0.85);
    vec3 c_yellow  = vec3(1.00, 0.90, 0.25);

    uint mode = param % 8u;

    if (mode == 0u) {
        // Mode 0: Polynomials & Rational Functions
        float y1 = a * (pos.x * pos.x * pos.x) - b * pos.x + c;
        float y1_r = a * pow(pos.x + dx, 3.0) - b * (pos.x + dx) + c;
        float y1_l = a * pow(pos.x - dx, 3.0) - b * (pos.x - dx) + c;
        float dy1 = (y1_r - y1_l) / (2.0 * dx);
        color = blend_over(color, draw_explicit_curve(pos.y, y1, dy1, c_cyan, 2.5, px, glow));

        float y2 = d / (1.0 + pos.x * pos.x);
        float y2_r = d / (1.0 + (pos.x + dx) * (pos.x + dx));
        float y2_l = d / (1.0 + (pos.x - dx) * (pos.x - dx));
        float dy2 = (y2_r - y2_l) / (2.0 * dx);
        color = blend_over(color, draw_explicit_curve(pos.y, y2, dy2, c_orange, 2.2, px, glow));

        float y3 = atan(pos.x);
        float dy3 = 1.0 / (1.0 + pos.x * pos.x);
        color = blend_over(color, draw_explicit_curve(pos.y, y3, dy3, c_green, 1.8, px, glow));
    }
    else if (mode == 1u) {
        // Mode 1: Waves, Modulations & Fourier Series Synthesis
        float y1 = a * sin(b * pos.x - 2.0 * t);
        float dy1 = a * b * cos(b * pos.x - 2.0 * t);
        color = blend_over(color, draw_explicit_curve(pos.y, y1, dy1, c_cyan, 2.5, px, glow));

        float dy2 = 0.0;
        float y2 = fourier_square_wave(pos.x, terms, dy2);
        color = blend_over(color, draw_explicit_curve(pos.y, y2, dy2, c_yellow, 2.2, px, glow));

        float y3 = sinc(pos.x);
        float dy3 = dsinc(pos.x);
        color = blend_over(color, draw_explicit_curve(pos.y, y3, dy3, c_magenta, 2.0, px, glow));
    }
    else if (mode == 2u) {
        // Mode 2: Damped Oscillations, Gaussians & Solitons
        float env = exp(-0.15 * pos.x * pos.x);
        float y1 = 2.0 * env * cos(3.0 * pos.x - 3.0 * t);
        float y1_r = 2.0 * exp(-0.15 * pow(pos.x + dx, 2.0)) * cos(3.0 * (pos.x + dx) - 3.0 * t);
        float y1_l = 2.0 * exp(-0.15 * pow(pos.x - dx, 2.0)) * cos(3.0 * (pos.x - dx) - 3.0 * t);
        float dy1 = (y1_r - y1_l) / (2.0 * dx);
        color = blend_over(color, draw_explicit_curve(pos.y, y1, dy1, c_cyan, 2.5, px, glow));

        float sigma = max(a * 0.5, 0.2);
        float y2 = gaussian(pos.x, 0.0, sigma);
        float dy2 = dgaussian(pos.x, 0.0, sigma);
        color = blend_over(color, draw_explicit_curve(pos.y, y2, dy2, c_green, 2.2, px, glow));

        float phase = 0.8 * (pos.x - 1.5 * t);
        float sech_val = sech(phase);
        float y3 = 2.5 * sech_val * sech_val;
        float y3_r = 2.5 * pow(sech(0.8 * (pos.x + dx - 1.5 * t)), 2.0);
        float y3_l = 2.5 * pow(sech(0.8 * (pos.x - dx - 1.5 * t)), 2.0);
        float dy3 = (y3_r - y3_l) / (2.0 * dx);
        color = blend_over(color, draw_explicit_curve(pos.y, y3, dy3, c_orange, 2.2, px, glow));
    }
    else if (mode == 3u) {
        // Mode 3: Implicit Algebraic Curves F(x, y) = 0
        float hx = pos.x;
        float hy = pos.y;
        float top = hx * hx + hy * hy - 1.0;
        float f_heart = top * top * top - hx * hx * hy * hy * hy;
        float f_hx = (pow((hx+dx)*(hx+dx) + hy*hy - 1.0, 3.0) - (hx+dx)*(hx+dx)*hy*hy*hy) -
                     (pow((hx-dx)*(hx-dx) + hy*hy - 1.0, 3.0) - (hx-dx)*(hx-dx)*hy*hy*hy);
        float f_hy = (pow(hx*hx + (hy+dy)*(hy+dy) - 1.0, 3.0) - hx*hx*pow(hy+dy, 3.0)) -
                     (pow(hx*hx + (hy-dy)*(hy-dy) - 1.0, 3.0) - hx*hx*pow(hy-dy, 3.0));
        vec2 grad_heart = vec2(f_hx / (2.0 * dx), f_hy / (2.0 * dy));
        color = blend_over(color, draw_implicit_curve(f_heart, grad_heart, c_magenta, 2.5, px, glow));

        float fa = 2.0 + sin(t);
        float f_folium = pow(pos.x, 3.0) + pow(pos.y, 3.0) - 3.0 * fa * pos.x * pos.y;
        vec2 grad_folium = vec2(
            3.0 * pos.x * pos.x - 3.0 * fa * pos.y,
            3.0 * pos.y * pos.y - 3.0 * fa * pos.x
        );
        color = blend_over(color, draw_implicit_curve(f_folium, grad_folium, c_cyan, 2.2, px, glow));

        float cc = 1.5;
        float ca = 1.6 + 0.6 * sin(t * 0.8);
        float p1 = (pos.x - cc)*(pos.x - cc) + pos.y * pos.y;
        float p2 = (pos.x + cc)*(pos.x + cc) + pos.y * pos.y;
        float f_cassini = p1 * p2 - pow(ca, 4.0);
        float f_cx = (((pos.x+dx - cc)*(pos.x+dx - cc) + pos.y*pos.y)*((pos.x+dx + cc)*(pos.x+dx + cc) + pos.y*pos.y)) -
                     (((pos.x-dx - cc)*(pos.x-dx - cc) + pos.y*pos.y)*((pos.x-dx + cc)*(pos.x-dx + cc) + pos.y*pos.y));
        float f_cy = (((pos.x - cc)*(pos.x - cc) + (pos.y+dy)*(pos.y+dy))*((pos.x + cc)*(pos.x + cc) + (pos.y+dy)*(pos.y+dy))) -
                     (((pos.x - cc)*(pos.x - cc) + (pos.y-dy)*(pos.y-dy))*((pos.x + cc)*(pos.x + cc) + (pos.y-dy)*(pos.y-dy)));
        vec2 grad_cassini = vec2(f_cx / (2.0 * dx), f_cy / (2.0 * dy));
        color = blend_over(color, draw_implicit_curve(f_cassini, grad_cassini, c_yellow, 2.2, px, glow));
    }
    else if (mode == 4u) {
        // Mode 4: Polar Curves r = f(theta)
        vec2 polar = cartesian_to_polar(pos);
        float r = polar.x;
        float theta = polar.y;

        float k_rose = max(float(terms % 12u), 2.0);
        float r_rose = a * abs(cos(k_rose * theta + 0.5 * t));
        float f_rose = r - r_rose;
        vec2 grad_rose = vec2(dFdx(f_rose), dFdy(f_rose)) / px;
        color = blend_over(color, draw_implicit_curve(f_rose, grad_rose, c_cyan, 2.5, px, glow));

        float r_card = b * (1.0 - cos(theta - t * 0.5));
        float f_card = r - r_card;
        vec2 grad_card = vec2(dFdx(f_card), dFdy(f_card)) / px;
        color = blend_over(color, draw_implicit_curve(f_card, grad_card, c_orange, 2.2, px, glow));
    }
    else if (mode == 5u) {
        // Mode 5: Taylor Series Polynomial Approximations
        float y_exact = sin(pos.x);
        float dy_exact = cos(pos.x);
        color = blend_over(color, draw_explicit_curve(pos.y, y_exact, dy_exact, c_cyan, 3.0, px, glow));

        float y_taylor = 0.0;
        float dy_taylor = 0.0;
        float term_val = pos.x;
        float dterm_val = 1.0;
        uint max_n = min(terms, 10u);

        for (uint n = 0u; n < max_n; ++n) {
            y_taylor += term_val;
            dy_taylor += dterm_val;

            float n1 = float(2u * n + 2u);
            float n2 = float(2u * n + 3u);
            dterm_val = -dterm_val * (pos.x * pos.x) * (n1 - 1.0) / (n1 * (n1 - 1.0));
            term_val = -term_val * (pos.x * pos.x) / (n1 * n2);
        }
        color = blend_over(color, draw_explicit_curve(pos.y, y_taylor, dy_taylor, c_yellow, 2.2, px, glow));
    }
    else if (mode == 6u) {
        // Mode 6: User Interactive Parametric Function
        float y_custom = a * sin(b * pos.x) * exp(-c * pos.x * pos.x) + d;
        float y_cr = a * sin(b * (pos.x + dx)) * exp(-c * (pos.x + dx) * (pos.x + dx)) + d;
        float y_cl = a * sin(b * (pos.x - dx)) * exp(-c * (pos.x - dx) * (pos.x - dx)) + d;
        float dy_custom = (y_cr - y_cl) / (2.0 * dx);
        color = blend_over(color, draw_explicit_curve(pos.y, y_custom, dy_custom, c_cyan, 3.0, px, glow));

        color = blend_over(color, draw_explicit_curve(pos.y, dy_custom, 0.0, c_magenta, 1.8, px, glow));
    }
    else {
        // Mode 7: Elliptic curve: y^2 = x^3 + a*x + b
        float f_ec = pos.y * pos.y - (pow(pos.x, 3.0) + a * pos.x + b);
        vec2 grad_ec = vec2(- (3.0 * pos.x * pos.x + a), 2.0 * pos.y);
        color = blend_over(color, draw_implicit_curve(f_ec, grad_ec, c_cyan, 2.5, px, glow));
    }

    // Optional Cursor & Tangent Inspection Point
    if ((u_flags & 4u) != 0u) {
        float d_cur = length(pos - u_cursor);
        float cur_circle = 1.0 - smoothstep(px * 4.0, px * 6.0, abs(d_cur - px * 7.0));
        color = mix(color, vec3(1.0, 1.0, 1.0), cur_circle * 0.9);
        float cur_dot = 1.0 - smoothstep(0.0, px * 3.0, d_cur);
        color = mix(color, c_yellow, cur_dot);
    }

    pixel_color = vec4(color, 1.0);
}
