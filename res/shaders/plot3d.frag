// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
#extension GL_GOOGLE_include_directive : require

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;
layout(location = 6) uniform float u_time;
layout(location = 8) uniform uint param;          // 3D preset selector

layout(location = 10) uniform vec4 u_params;      // custom parameters (a, b, c, d)
layout(location = 12) uniform uint u_flags;       // bit 0: grid, bit 1: axes, bit 3: neon glow

layout(location = 14) uniform vec2 u_cam_rot;     // 3D camera (yaw, pitch)
layout(location = 15) uniform float u_cam_dist;   // 3D camera distance

layout(location = 0) out vec4 pixel_color;

#include "math_functions.glsl"

// -----------------------------------------------------------------------------
// 3D Mathematical Surface & Heightfield Evaluation
// -----------------------------------------------------------------------------
float eval_surface_3d(vec2 xz, float t, uint mode, vec4 p) {
    float a = p.x;
    float b = p.y;
    float c = p.z;
    float d = p.w;

    if (mode == 0u) {
        // Mode 0: Animated 3D Radial Ripple / Wave Equation
        float r = length(xz);
        return a * 1.6 * sin(b * r - 3.5 * t) * exp(-0.06 * r * r);
    }
    else if (mode == 1u) {
        // Mode 1: Moving Standing Wave / Vibrating Membrane
        return a * 0.9 * (sin(b * xz.x - 2.0 * t) * cos(b * xz.y) +
                          cos(b * xz.x) * sin(b * xz.y + 2.0 * t));
    }
    else if (mode == 2u) {
        // Mode 2: 3D Soliton Wave Collision
        float s1 = sech(0.7 * (xz.x - 1.8 * t));
        float s2 = sech(0.7 * (xz.y - 1.8 * t));
        return a * 2.2 * (s1 * s1 + s2 * s2);
    }
    else if (mode == 3u) {
        // Mode 3: Animated Monkey Saddle: z = x^3 - 3*x*y^2
        float angle = 0.25 * t;
        vec2 r_xz = vec2(xz.x * cos(angle) - xz.y * sin(angle),
                         xz.x * sin(angle) + xz.y * cos(angle));
        return a * 0.035 * (r_xz.x * r_xz.x * r_xz.x - 3.0 * r_xz.x * r_xz.y * r_xz.y);
    }
    else if (mode == 4u) {
        // Mode 4: Modulating Gaussian Potential Well / Hill
        float r2 = dot(xz, xz);
        return a * 2.8 * exp(-0.25 * r2) * cos(2.2 * sqrt(r2) - 3.0 * t);
    }
    else if (mode == 5u) {
        // Mode 5: Quantum 2D Wavepacket
        return a * 1.3 * sin(1.4 * xz.x - 2.5 * t) * sin(1.4 * xz.y - 2.5 * t) * exp(-0.08 * dot(xz, xz));
    }
    else if (mode == 6u) {
        // Mode 6: User Parametric Surface
        return a * sin(b * xz.x) * cos(c * xz.y + t) + d * 0.2;
    }
    else {
        // Mode 7: Bessel J0 radial wave approximation
        float r = length(xz);
        return a * 1.5 * cos(b * r - 2.5 * t) / (1.0 + 0.3 * r);
    }
}

void main()
{
    float t = u_time;
    uint mode = param % 8u;
    bool show_grid = (u_flags & 1u) != 0u;
    bool glow_on   = (u_flags & 8u) != 0u;

    vec2 sc = (gl_FragCoord.xy * 2.0 - vec2(rect_width, rect_height)) / min(rect_width, rect_height);

    // Camera setup
    float yaw = u_cam_rot.x;
    float pitch = u_cam_rot.y;
    float dist = u_cam_dist;

    vec3 ro = vec3(
        dist * cos(pitch) * sin(yaw),
        dist * sin(pitch),
        dist * cos(pitch) * cos(yaw)
    );
    vec3 ta = vec3(0.0, 0.0, 0.0);

    vec3 ww = normalize(ta - ro);
    vec3 uu = normalize(cross(ww, vec3(0.0, 1.0, 0.0)));
    vec3 vv = normalize(cross(uu, ww));
    vec3 rd = normalize(sc.x * uu + sc.y * vv + 1.85 * ww);

    // Deep space dark background with subtle vignette
    vec3 bg_color = vec3(0.04, 0.05, 0.08) * (1.0 - 0.3 * length(sc));

    // Raymarch bounds
    float t_min = 1.0;
    float t_max = 50.0;
    float t_step = 0.22;
    float t_ray = t_min;

    bool hit = false;
    vec3 hit_pos = vec3(0.0);

    float prev_diff = 0.0;
    float prev_t = t_min;

    for (int i = 0; i < 180; ++i) {
        vec3 pos = ro + rd * t_ray;
        if (abs(pos.x) > 12.0 || abs(pos.z) > 12.0) {
            t_ray += t_step;
            if (t_ray > t_max) break;
            continue;
        }

        float h = eval_surface_3d(pos.xz, t, mode, u_params);
        float diff = pos.y - h;

        if (i > 0 && diff * prev_diff < 0.0) {
            // Binary search refinement
            float t_a = prev_t;
            float t_b = t_ray;
            for (int k = 0; k < 6; ++k) {
                float t_mid = 0.5 * (t_a + t_b);
                vec3 p_mid = ro + rd * t_mid;
                float h_mid = eval_surface_3d(p_mid.xz, t, mode, u_params);
                if ((p_mid.y - h_mid) * prev_diff < 0.0) {
                    t_b = t_mid;
                } else {
                    t_a = t_mid;
                }
            }
            hit = true;
            hit_pos = ro + rd * (0.5 * (t_a + t_b));
            break;
        }

        prev_diff = diff;
        prev_t = t_ray;
        t_ray += t_step;
        if (t_ray > t_max) break;
    }

    if (!hit) {
        // Ground grid for 3D depth perception
        float t_ground = -ro.y / rd.y;
        if (t_ground > 0.0 && t_ground < 60.0) {
            vec3 g_pos = ro + rd * t_ground;
            if (abs(g_pos.x) < 14.0 && abs(g_pos.z) < 14.0) {
                vec2 g_grid = abs(fract(g_pos.xz * 0.5 + 0.5) - 0.5);
                float line_d = min(g_grid.x, g_grid.y);
                float line_a = smoothstep(0.04, 0.0, line_d) * exp(-0.05 * t_ground);
                bg_color = mix(bg_color, vec3(0.18, 0.22, 0.30), line_a * 0.5);
            }
        }
        pixel_color = vec4(bg_color, 1.0);
        return;
    }

    // Compute surface normal via central difference
    float eps = 0.015;
    float h_xp = eval_surface_3d(hit_pos.xz + vec2(eps, 0.0), t, mode, u_params);
    float h_xm = eval_surface_3d(hit_pos.xz - vec2(eps, 0.0), t, mode, u_params);
    float h_zp = eval_surface_3d(hit_pos.xz + vec2(0.0, eps), t, mode, u_params);
    float h_zm = eval_surface_3d(hit_pos.xz - vec2(0.0, eps), t, mode, u_params);

    float dhdx = (h_xp - h_xm) / (2.0 * eps);
    float dhdz = (h_zp - h_zm) / (2.0 * eps);
    vec3 norm = normalize(vec3(-dhdx, 1.0, -dhdz));

    // Surface Lighting
    vec3 light_dir = normalize(vec3(0.5, 1.2, 0.7));
    vec3 view_dir = -rd;

    float diff_light = max(dot(norm, light_dir), 0.0);
    vec3 half_vec = normalize(light_dir + view_dir);
    float spec_light = pow(max(dot(norm, half_vec), 0.0), 32.0);
    float fresnel = pow(1.0 - max(dot(norm, view_dir), 0.0), 3.0);

    // Height-based colormap
    float height_val = hit_pos.y;
    vec3 col_low  = vec3(0.10, 0.40, 0.85); // Azure
    vec3 col_mid  = vec3(0.15, 0.85, 0.75); // Cyan
    vec3 col_high = vec3(1.00, 0.60, 0.15); // Amber

    vec3 surface_color;
    if (height_val < 0.0) {
        surface_color = mix(col_low, col_mid, clamp(height_val * 0.5 + 1.0, 0.0, 1.0));
    } else {
        surface_color = mix(col_mid, col_high, clamp(height_val * 0.5, 0.0, 1.0));
    }

    // Shading composition
    vec3 lit_color = surface_color * (0.25 + 0.65 * diff_light) +
                     vec3(1.0) * (0.4 * spec_light) +
                     vec3(0.3, 0.8, 1.0) * (0.35 * fresnel);

    // 3D Neon Wireframe / Isolines Overlay
    if (show_grid) {
        vec2 grid_uv = abs(fract(hit_pos.xz * 1.0 + 0.5) - 0.5);
        float d_wire = min(grid_uv.x, grid_uv.y);
        float wire_alpha = smoothstep(0.06, 0.0, d_wire);

        vec3 wire_color = vec3(0.9, 0.98, 1.0);
        if (glow_on) {
            float glow_wire = exp(-d_wire * 18.0) * 1.2;
            lit_color += (wire_color * 1.5 + surface_color * 0.8) * glow_wire;
        } else {
            lit_color = mix(lit_color, wire_color, wire_alpha * 0.7);
        }
    }

    // Distance atmospheric fog
    float fog = 1.0 - exp(-0.03 * length(hit_pos - ro));
    lit_color = mix(lit_color, bg_color, clamp(fog, 0.0, 1.0));

    pixel_color = vec4(lit_color, 1.0);
}
