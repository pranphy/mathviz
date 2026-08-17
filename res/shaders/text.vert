// -*- coding: glsl -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// Text vertex shader — compiled at runtime from source (not SPIR-V)
// so that the text module remains self-contained.

#version 460 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;

out vec2  v_uv;
out vec4  v_color;

uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * vec4(a_pos, 0.0, 1.0);
    v_uv    = a_uv;
    v_color = a_color;
}
