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
layout(location = 8) uniform uint param;

layout(location = 0) out vec4 pixel_color;

vec2 mult(vec2 a, vec2 b){
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
vec2 div(vec2 a, vec2 b){
    return 1.0/(b.x * b.x + b.y * b.y) * mult(vec2(b.x, -b.y),a);
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
            result = mult(result, base);
        }
        base = mult(base, base);
        n >>= 1;
    }

    if (neg) {
        return div(vec2(1.0, 0.0), result);
    }

    return result;
}

vec2 ratio(vec2 z, int n){
    // f(z) = z^n - 1
    // f'(z) = n * z^(n-1)
    vec2 zn = complex_pow(z, n);
    vec2 zn_minus_1 = complex_pow(z, n - 1);

    return div(zn - vec2(1.0, 0.0), float(n) * zn_minus_1);
}

void main()
{
    vec2 Z = vec2(gl_FragCoord.x * (area_w.y - area_w.x) / rect_width  + area_w.x,
                  gl_FragCoord.y * (area_h.y - area_h.x) / rect_height + area_h.x);

    //int n = 5; // Or pass 'n' as a uniform integer from your application!
    int n = int(param);
    uint iteration = 0;
    float tol = 0.01;

    pixel_color = vec4(0.1, 0.1, 0.1, 1.0);

    while (iteration < 2.0 * float(max_iterations))
    {
        Z = Z - ratio(Z, n);

        // Check convergence: if Z is close to the unit circle (magnitude approx 1)
        if (abs(dot(Z, Z) - 1.0) < tol || iteration >= 2U * max_iterations) {
            // Find which root it landed on using its angle (atan)
            float angle = atan(Z.y, Z.x);
            if (angle < 0.0) angle += 6.28318530718; // 2 * PI

            float sector = 6.28318530718 / float(n);
            int root_index = int(round(angle / sector)) % n;

            // Generate a distinct color dynamically based on the root index
            float hue = float(root_index) / float(n);
            pixel_color = vec4(hue, 1.0 - hue, 0.5 + 0.5 * hue, 1.0);
            break;
        }

        ++iteration;
    }
}
