// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core
// Cosine color palette for vibrant cycling colors

layout(location = 0) uniform float rect_width;
layout(location = 1) uniform float rect_height;
layout(location = 6) uniform float u_time; // Time uniform in seconds passed from CPU

layout(location = 0) out vec4 pixel_color;

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.0, 0.33, 0.67);
    return a + b * cos(6.28318 * (c * t + d));
}

void main() {
    // Normalize and scale coordinates to a mathematical grid (e.g., x from -4 to 4)
    vec2 u_resolution = vec2(rect_width, rect_height);
    vec2 st = (gl_FragCoord.xy * 2.0 - u_resolution.xy) / min(u_resolution.x, u_resolution.y);
    st *= 4.0; 

    vec3 finalColor = vec3(0.0);
    float num = 3.0;

    // Plot multiple overlapping, animated functions using a loop
    for (float i = 0.0; i < num; i++) {
        float x = st.x + i * 0.4;
        
        // Define an arbitrary mathematical function f(x) with time animation
        // Example: Damped wave modulated by trigonometric functions
        float freq = 1.5 + i * 0.2;
        float speed = u_time * (1.0 + i * 0.3);
        float z = (-1.0 + mod(i,num));
        float y = abs(z)*sin(x * freq + u_time *z ) + (1.0-abs(z))*(sin(x * freq + u_time *(abs(z)-1.0)) + sin(x * freq + u_time *(abs(z)+1.0) )) ; // * cos(x * 0.5 - speed * 0.5) * exp(-0.15 * abs(x));


        // Calculate distance from pixel's y-coordinate to the function's y-value
        float d = abs(st.y - y);

        // Neon glow effect (inverse power trick)
        d = pow(0.04 / d, 1.3);

        // Assign animated colors from the palette
        vec3 col = palette(x * 0.1 + u_time * 0.15 + i * 0.3);

        // Accumulate light intensity
        finalColor += col * d;
    }

    // Optional: Add a faint background grid for a "graph paper" look
    vec2 grid = abs(fract(st - 0.5) - 0.5) / fwidth(st);
    float line = min(grid.x, grid.y);
    vec3 gridCol = vec3(0.08) * (1.0 - min(line, 1.0));
    finalColor += gridCol;

    pixel_color = vec4(finalColor, 1.0);
}
