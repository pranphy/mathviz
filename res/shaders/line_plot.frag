// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 9) uniform vec4 u_color; // Series line color (r, g, b, a)

layout(location = 0) out vec4 pixel_color;

void main()
{
    pixel_color = u_color;
}
