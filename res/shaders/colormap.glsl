// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]

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

vec3 get_smooth_color(uint iteration, float z_sq_dist, uint max_iterations) {
    if (iteration == max_iterations) {
        return vec3(0.0);
    }

    // Smooth coloring using hardware-accelerated log2
    float log_zn = log2(z_sq_dist) * 0.5;
    float nu = log2(log_zn);
    float smooth_iter = float(iteration) + 1.0 - nu;

    // Map the smooth iteration count to our color map range
    float color_pos = mod(smooth_iter * 0.2, float(color_map.length()));

    int idx1 = int(floor(color_pos));
    int idx2 = (idx1 + 1) % color_map.length();
    float fract_part = fract(color_pos);

    return mix(color_map[idx1], color_map[idx2], fract_part);
}

vec3 cosine_palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.0, 0.33, 0.67);
    return a + b * cos(6.28318 * (c * t + d));
}
