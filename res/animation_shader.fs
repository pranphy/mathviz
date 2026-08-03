// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]
// date   : 2025-12-23

#version 330 core

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 2) uniform vec2 area_w;
layout(location = 3) uniform vec2 area_h;
layout(location = 4) uniform uint max_iterations;


out vec4 fragColor;

in vec2 fragCoord;          // from vertex shader

uniform vec2  u_resolution; // window size
uniform float u_time;       // time in seconds

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);

    return a + b * cos(6.28318 * (c * t + d));
}

void main()
{
    vec2 uv = (fragCoord * 2.0 - u_resolution.xy) / u_resolution.y;
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

    fragColor = vec4(finalColor, 1.0);
}

