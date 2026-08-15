// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]

#version 460

#extension GL_GOOGLE_include_directive : require

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;
layout(location = 8) uniform uint pol_deg;
layout(location = 13) uniform vec2 root0;
layout(location = 14) uniform vec2 root1;
layout(location = 15) uniform vec2 root2;
layout(location = 16) uniform vec2 root3;
layout(location = 17) uniform vec2 root4;
layout(location=20)uniform vec2 coef0;
layout(location=21)uniform vec2 coef1;
layout(location=22)uniform vec2 coef2;
layout(location=23)uniform vec2 coef3;
layout(location=24)uniform vec2 coef4;
layout(location=25)uniform vec2 coef5;

layout(location=30) uniform float show_grid;

layout(location = 0) out vec4 pixel_color;

vec4 color0 = vec4(0.0f,1.0f,0.0f,1.0f);
vec4 color1 = color0;
vec4 color2 = color0;
vec4 color3 = color0;
vec4 color4 = color0;

const int MAX_DEGREE = 5;
const float CLOSE_ENOUGH = 1e-3;

#include "colormap.glsl"

vec2 complex_mult(vec2 a, vec2 b){
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
vec2 complex_div(vec2 a, vec2 b){
    return 1.0/(b.x * b.x + b.y * b.y) * complex_mult(vec2(b.x, -b.y),a);
}

//#include "colormap.glsl"
vec2 complex_pow(vec2 z, int n) {
    if (n == 0) return vec2(1.0, 0.0);

    // Handle negative exponents
    bool neg = n < 0;
    if (neg) n = -n;

    vec2 result = vec2(1.0, 0.0);
    vec2 base = z;

    while (n > 0) {
        if ((n & 1) != 0) {
            result = complex_mult(result, base);
        }
        base = complex_mult(base, base);
        n >>= 1;
    }

    if (neg) {
        return complex_div(vec2(1.0, 0.0), result);
    }

    return result;
}


vec2 poly(vec2 z, vec2[MAX_DEGREE + 1] coefs){
    vec2 result = vec2(0.0);
    for(int n = 0; n < int(pol_deg) + 1; n++){
        result += complex_mult(coefs[n], complex_pow(z, n));
    }
    return result;
}

vec2 dpoly(vec2 z, vec2[MAX_DEGREE + 1] coefs){
    vec2 result = vec2(0.0);
    for(int n = 1; n < int(pol_deg) + 1; n++){
        result += n * complex_mult(coefs[n], complex_pow(z, n - 1));
    }
    return result;
}


vec2 seek_root(vec2 z, vec2[MAX_DEGREE + 1] coefs, int max_steps, out int steps_t){
    float last_len;
    float curr_len;
    float threshold = CLOSE_ENOUGH;

    for(int i = 0; i < max_steps; i++){
        last_len = curr_len;
        steps_t = i;
        vec2 step = complex_div(poly(z, coefs), dpoly(z, coefs));
        curr_len = length(step);
        if(curr_len < threshold){
            break;
        }
        z = z - step;
    }

    return z;
}

vec2 get_z(){
    vec2 Z = vec2(gl_FragCoord.x * (area_w.y - area_w.x) / rect_width  + area_w.x,
                  gl_FragCoord.y * (area_h.y - area_h.x) / rect_height + area_h.x);
    return Z;

}

void main() {
    vec2[MAX_DEGREE + 1] coefs = vec2[MAX_DEGREE + 1](coef0, coef1, coef2, coef3, coef4, coef5);
    vec2[MAX_DEGREE] roots = vec2[MAX_DEGREE](root0, root1, root2, root3, root4);
    //vec4[MAX_DEGREE] colors = vec4[MAX_DEGREE](color0, color1, color2, color3, color4);
    pixel_color = vec4(0.0,0.0,0.0, 1.0);

    float julia_highlight = 0.0;
    float black_for_cycles = 1.0;

    vec2 z = get_z();
    vec2 xyz_coords = z;

    int n_steps = int(max_iterations);
    int steps_t = 10;

    vec2 found_root = seek_root(z, coefs, int(n_steps), steps_t);

    float dist;
    for(int i = 0; i < int(pol_deg); i++){
        dist = distance(roots[i], found_root);
        if(abs(found_root.x - roots[i].x) < 1e-2 && abs(found_root.y - roots[i].y) < 1e-2){
            int root_index = i;
            // Generate a distinct color dynamically based on the root index
            //float hue = float(root_index+1) / float(pol_deg+1.0);
            //pixel_color = vec4(hue, 1.0 - hue, 0.5 + 0.5 * hue, 1.0);
            //pixel_color = vec4(cosine_palette(i/float(pol_deg)),1.0f);
            pixel_color = vec4(cosine_palette(i/float(pol_deg)+ steps_t/float(n_steps)),1.0f);
        }
    }

    // Draw tiny circles at the root locations
    float pixel_size = (area_w.y - area_w.x) / rect_width;
    for(int i = 0; i < int(pol_deg); i++){
        float d = distance(z, roots[i]);
        float pixel_dist = d / pixel_size;
        if(pixel_dist < 6.5){
            if(pixel_dist > 4.5){
                pixel_color = vec4(0.0, 0.0, 0.0, 1.0); // Black border
            } else {
                pixel_color = vec4(0.8, 1.0, 0.3, 1.0); // White fill
            }
        }
    }

    if(show_grid > 0.0){
        vec2 grid = abs(fract(z - 0.5) - 0.5) / fwidth(z);
        float line = min(grid.x, grid.y);
        vec3 gridCol = vec3(0.08) * (1.0 - min(line, 1.0));
        pixel_color += vec4(gridCol,1.0f);
    }
 }
