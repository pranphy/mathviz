// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 6) uniform float u_time; // Time uniform in seconds passed from CPU

layout(location = 0) out vec4 pixel_color;

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);

    return a + b * cos(6.28318 * (c * t + d));
}

void main()
{


    // Normalize coordinates to [-1, 1] relative to the viewport center, preserving aspect ratio
    vec2 uv = (gl_FragCoord.xy * 2.0 - vec2(rect_width, rect_height)) / min(rect_width, rect_height);
    vec2 uv0 = uv;
    vec3 finalColor = vec3(0.0);

    for (float i = 0.0; i < 4.0; i++) {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));

        vec3 col = palette(length(uv0) + i * 0.4 + u_time * 0.4);

        d = sin(d * 8.0 + u_time) / 8.0;
        d = abs(d);

        d = pow(0.01 / d, 1.2);

        finalColor += col * d;
    }

    pixel_color = vec4(finalColor, 1.0);

}
