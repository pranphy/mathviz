// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w; // Contains X bounds (center maps to rotation angle X)
layout(location = 3) uniform vec2 area_h; // Contains Y bounds (center maps to rotation angle Y)
layout(location = 4) uniform uint max_iterations; // Reused here for max raymarch steps
layout(location = 6) uniform float u_time; // Time uniform in seconds passed from CPU

layout(location = 0) out vec4 pixel_color;

// Rotation helpers
vec3 rotateY(vec3 p, float a) {
    float c = cos(a), s = sin(a);
    return vec3(c * p.x - s * p.z, p.y, s * p.x + c * p.z);
}

vec3 rotateX(vec3 p, float a) {
    float c = cos(a), s = sin(a);
    return vec3(p.x, c * p.y - s * p.z, s * p.y + c * p.z);
}

// Torus Signed Distance Field (SDF)
float sdTorus(vec3 p, vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

// Scene SDF map
float map(vec3 p) {
    // Spin the torus slowly over time around X and Y axes
    vec3 q = rotateY(rotateX(p, u_time * 0.6), u_time * 0.4);
    return sdTorus(q, vec2(0.65, 0.25));
}

// Calculate normal vector on the surface
vec3 getNormal(vec3 p) {
    float d = map(p);
    vec2 e = vec2(0.001, 0.0);
    vec3 n = d - vec3(
        map(p - e.xyy),
        map(p - e.yxy),
        map(p - e.yyx)
    );
    return normalize(n);
}

// Raymarching algorithm
float raymarch(vec3 ro, vec3 rd, uint max_steps) {
    float dO = 0.0;
    for (uint i = 0; i < max_steps; i++) {
        vec3 p = ro + rd * dO;
        float dS = map(p);
        dO += dS;
        if (dO > 15.0 || dS < 0.0005) break;
    }
    return dO;
}

void main()
{
    // Normalized coordinates to [-1, 1], preserving aspect ratio
    vec2 uv = (gl_FragCoord.xy * 2.0 - vec2(rect_width, rect_height)) / min(rect_width, rect_height);

    // Map scene navigation to camera positioning:
    // Panning X/Y maps to camera orbit angles, scale maps to camera distance
    float angle_x = (area_w.x + area_w.y) * 0.5; // Orbit angle around Y
    float angle_y = (area_h.x + area_h.y) * 0.5; // Orbit angle around X
    
    // Bounds check scale to avoid clipping inside the torus
    float zoom_scale = (area_w.y - area_w.x) / 3.0; 
    float dist = max(zoom_scale * 2.2, 1.2);

    // Compute ray origin (camera position) using spherical coordinates
    vec3 ro = vec3(dist * sin(angle_x) * cos(angle_y),
                   dist * sin(angle_y),
                   dist * cos(angle_x) * cos(angle_y));
                   
    // Set up look-at camera matrix
    vec3 target = vec3(0.0);
    vec3 f = normalize(target - ro);
    vec3 r = normalize(cross(vec3(0.0, 1.0, 0.0), f));
    vec3 u = cross(f, r);
    vec3 rd = normalize(f + uv.x * r + uv.y * u);

    // Raymarch
    uint steps = max_iterations * 2; // scale step detail with iterations slider
    if (steps < 40) steps = 40;
    if (steps > 150) steps = 150;
    
    float d = raymarch(ro, rd, steps);

    vec3 final_color = vec3(0.05); // Dark grey background

    if (d < 15.0) {
        // Hit point
        vec3 p = ro + rd * d;
        vec3 n = getNormal(p);

        // Dynamic local torus coordinates color mapping
        vec3 local_p = rotateY(rotateX(p, u_time * 0.6), u_time * 0.4);
        float phi = atan(local_p.z, local_p.x);
        vec3 base_color = 0.5 + 0.5 * cos(phi + u_time + vec3(0.0, 2.0, 4.0));

        // Diffuse + Specular lighting
        vec3 light_pos1 = vec3(2.0, 4.0, 3.0);
        vec3 light_dir1 = normalize(light_pos1 - p);
        float diff1 = clamp(dot(n, light_dir1), 0.0, 1.0);
        
        // Shiny specular highlights
        vec3 ref1 = reflect(-light_dir1, n);
        float spec1 = pow(clamp(dot(ref1, -rd), 0.0, 1.0), 32.0);

        // Fill light
        vec3 light_pos2 = vec3(-3.0, -2.0, -3.0);
        vec3 light_dir2 = normalize(light_pos2 - p);
        float diff2 = clamp(dot(n, light_dir2), 0.0, 1.0) * 0.3;

        // Shadow mapping for light 1
        float shadow_d = raymarch(p + n * 0.01, light_dir1, steps / 2);
        if (shadow_d < length(light_pos1 - p)) {
            diff1 *= 0.2;
            spec1 = 0.0;
        }

        final_color = base_color * (diff1 + diff2 + vec3(0.1)) + vec3(0.7) * spec1;
        
        // Simple fog/depth shading
        final_color = mix(final_color, vec3(0.05), 1.0 - exp(-0.04 * d * d));
    }

    pixel_color = vec4(final_color, 1.0);
}
