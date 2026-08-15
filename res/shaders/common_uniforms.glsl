// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#ifndef COMMON_UNIFORMS_GLSL
#define COMMON_UNIFORMS_GLSL

// Centralized OpenGL Uniform Layout Locations
layout(location = 0)  uniform float rect_width;
layout(location = 1)  uniform float rect_height;
layout(location = 2)  uniform vec2  area_w;
layout(location = 3)  uniform vec2  area_h;
layout(location = 4)  uniform uint  max_iterations;
layout(location = 6)  uniform float u_time;
layout(location = 8)  uniform uint  u_param;
layout(location = 9)  uniform vec3  u_color;     // Custom per-layer curve color
layout(location = 10) uniform vec4  u_params;    // Parameters (a, b, c, d)
layout(location = 11) uniform vec2  u_cursor;    // Cursor world math position (x, y)
layout(location = 12) uniform uint  u_flags;     // Bitmask: bit 0 = grid, bit 1 = axes, bit 2 = cursor, bit 3 = glow
layout(location = 14) uniform vec2  u_cam_rot;   // 3D camera rotation (yaw, pitch)
layout(location = 15) uniform float u_cam_dist;  // 3D camera distance

#endif
