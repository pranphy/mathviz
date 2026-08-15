// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
#extension GL_GOOGLE_include_directive : require

#include "../common_uniforms.glsl"
#include "../math_functions.glsl"

layout(location = 0) out vec4 pixel_color;

float eval_surface(vec2 xz, float t, vec4 p) {
    float r = length(xz);
    return p.x * 1.6 * sin(p.y * r - 3.5 * t) * exp(-0.06 * r * r);
}

void main()
{
    float t = u_time;
    bool show_grid = (u_flags & 1u) != 0u;
    bool glow_on   = (u_flags & 8u) != 0u;

    vec2 sc = (gl_FragCoord.xy * 2.0 - vec2(rect_width, rect_height)) / min(rect_width, rect_height);

    float yaw = u_cam_rot.x;
    float pitch = u_cam_rot.y;
    float dist = u_cam_dist;

    vec3 ro = vec3(dist * cos(pitch) * sin(yaw), dist * sin(pitch), dist * cos(pitch) * cos(yaw));
    vec3 ta = vec3(0.0, 0.0, 0.0);
    vec3 ww = normalize(ta - ro);
    vec3 uu = normalize(cross(ww, vec3(0.0, 1.0, 0.0)));
    vec3 vv = normalize(cross(uu, ww));
    vec3 rd = normalize(sc.x * uu + sc.y * vv + 1.85 * ww);

    vec3 bg_color = vec3(0.04, 0.05, 0.08) * (1.0 - 0.3 * length(sc));

    float t_ray = 1.0;
    float t_step = 0.22;
    bool hit = false;
    vec3 hit_pos = vec3(0.0);
    float prev_diff = 0.0, prev_t = 1.0;

    for (int i = 0; i < 180; ++i) {
        vec3 pos = ro + rd * t_ray;
        if (abs(pos.x) > 12.0 || abs(pos.z) > 12.0) {
            t_ray += t_step;
            if (t_ray > 50.0) break;
            continue;
        }

        float h = eval_surface(pos.xz, t, u_params);
        float diff = pos.y - h;

        if (i > 0 && diff * prev_diff < 0.0) {
            float t_a = prev_t, t_b = t_ray;
            for (int k = 0; k < 6; ++k) {
                float t_mid = 0.5 * (t_a + t_b);
                vec3 p_mid = ro + rd * t_mid;
                if ((p_mid.y - eval_surface(p_mid.xz, t, u_params)) * prev_diff < 0.0) t_b = t_mid;
                else t_a = t_mid;
            }
            hit = true;
            hit_pos = ro + rd * (0.5 * (t_a + t_b));
            break;
        }
        prev_diff = diff;
        prev_t = t_ray;
        t_ray += t_step;
        if (t_ray > 50.0) break;
    }

    if (!hit) {
        float t_ground = -ro.y / rd.y;
        if (t_ground > 0.0 && t_ground < 60.0) {
            vec3 g_pos = ro + rd * t_ground;
            if (abs(g_pos.x) < 14.0 && abs(g_pos.z) < 14.0) {
                vec2 g_grid = abs(fract(g_pos.xz * 0.5 + 0.5) - 0.5);
                float line_a = smoothstep(0.04, 0.0, min(g_grid.x, g_grid.y)) * exp(-0.05 * t_ground);
                bg_color = mix(bg_color, vec3(0.18, 0.22, 0.30), line_a * 0.5);
            }
        }
        pixel_color = vec4(bg_color, 1.0);
        return;
    }

    float eps = 0.015;
    float h_xp = eval_surface(hit_pos.xz + vec2(eps, 0.0), t, u_params);
    float h_xm = eval_surface(hit_pos.xz - vec2(eps, 0.0), t, u_params);
    float h_zp = eval_surface(hit_pos.xz + vec2(0.0, eps), t, u_params);
    float h_zm = eval_surface(hit_pos.xz - vec2(0.0, eps), t, u_params);
    vec3 norm = normalize(vec3(-(h_xp - h_xm) / (2.0 * eps), 1.0, -(h_zp - h_zm) / (2.0 * eps)));

    vec3 light_dir = normalize(vec3(0.5, 1.2, 0.7));
    float diff_light = max(dot(norm, light_dir), 0.0);
    float spec_light = pow(max(dot(norm, normalize(light_dir - rd)), 0.0), 32.0);
    float fresnel = pow(1.0 - max(dot(norm, -rd), 0.0), 3.0);

    vec3 col_low  = vec3(0.10, 0.40, 0.85);
    vec3 col_mid  = vec3(0.15, 0.85, 0.75);
    vec3 col_high = vec3(1.00, 0.60, 0.15);
    vec3 surface_color = (hit_pos.y < 0.0) ? mix(col_low, col_mid, clamp(hit_pos.y * 0.5 + 1.0, 0.0, 1.0))
                                           : mix(col_mid, col_high, clamp(hit_pos.y * 0.5, 0.0, 1.0));

    vec3 lit_color = surface_color * (0.25 + 0.65 * diff_light) + vec3(1.0) * (0.4 * spec_light) + vec3(0.3, 0.8, 1.0) * (0.35 * fresnel);

    if (show_grid) {
        vec2 grid_uv = abs(fract(hit_pos.xz * 1.0 + 0.5) - 0.5);
        float d_wire = min(grid_uv.x, grid_uv.y);
        vec3 wire_color = vec3(0.9, 0.98, 1.0);
        if (glow_on) {
            lit_color += (wire_color * 1.5 + surface_color * 0.8) * exp(-d_wire * 18.0) * 1.2;
        } else {
            lit_color = mix(lit_color, wire_color, smoothstep(0.06, 0.0, d_wire) * 0.7);
        }
    }

    float fog = 1.0 - exp(-0.03 * length(hit_pos - ro));
    lit_color = mix(lit_color, bg_color, clamp(fog, 0.0, 1.0));

    pixel_color = vec4(lit_color, 1.0);
}
