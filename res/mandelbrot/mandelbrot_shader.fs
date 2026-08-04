// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]

#version 460

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;

out vec4 pixel_color;

const vec3 color_map[] = {
    vec3(0.0,  0.0,  0.0),
    vec3(0.26, 0.18, 0.06),
    vec3(0.1,  0.03, 0.1),
    vec3(0.04, 0.0,  0.18),
    vec3(0.02, 0.02, 0.29),
    vec3(0.0,  0.03, 0.39),
    vec3(0.05, 0.17, 0.54),
    vec3(0.09, 0.32, 0.69),
    vec3(0.22, 0.49, 0.82),
    vec3(0.52, 0.71, 0.9),
    vec3(0.82, 0.92, 0.97),
    vec3(0.94, 0.91, 0.75),
    vec3(0.97, 0.79, 0.37),
    vec3(1.0,  0.67, 0.0),
    vec3(0.8,  0.5,  0.0),
    vec3(0.6,  0.34, 0.0),
    vec3(0.41, 0.2,  0.01)
};

void main()
{
    vec2 C = vec2(gl_FragCoord.x * (area_w.y - area_w.x) / rect_width  + area_w.x,
                  gl_FragCoord.y * (area_h.y - area_h.x) / rect_height + area_h.x);
    vec2 Z = vec2(0.0);
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

    if (iteration == max_iterations)
    {
        pixel_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Continuous potential (smooth coloring) formula to remove banding
        float log_zn = log(z_sq_dist) / 2.0;
        float nu = log(log_zn / 0.693147) / 0.693147; // log(2.0) approx 0.693147
        float smooth_iter = float(iteration) + 1.0 - nu;

        // Map the smooth iteration count to our color map range (17 colors)
        float color_pos = mod(smooth_iter * 0.2, float(color_map.length()));
        
        int idx1 = int(floor(color_pos));
        int idx2 = (idx1 + 1) % color_map.length();
        float fract_part = fract(color_pos);

        // Interpolate smoothly between the two palette colors
        vec3 final_color = mix(color_map[idx1], color_map[idx2], fract_part);
        pixel_color = vec4(final_color, 1.0);
    }
}// date   : 2025-12-23

