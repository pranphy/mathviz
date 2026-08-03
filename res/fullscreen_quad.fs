// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl
// author : Prakash [प्रकाश]
// date   : 2025-12-24

#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 fragCoord;

uniform vec2 u_resolution;

void main()
{
    fragCoord = (aPos * 0.5 + 0.5) * u_resolution;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

