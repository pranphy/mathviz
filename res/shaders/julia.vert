// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]
// date   : 2026-08-05

#version 460 core

layout(location = 0) in vec3 vertex_position;
void main()
{
    gl_Position = vec4(vertex_position, 1.0);
}
