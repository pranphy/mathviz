// -*- coding: utf-8 -*-
// vim: ai ts=4 sts=4 et sw=4 ft=glsl

#version 460 core

layout(location = 0) in vec2 a_pos; // World math coordinates (x, y)

layout(location = 2) uniform vec2 area_w; // [x_min, x_max]
layout(location = 3) uniform vec2 area_h; // [y_min, y_max]

void main()
{
    float ndc_x = ((a_pos.x - area_w.x) / (area_w.y - area_w.x)) * 2.0 - 1.0;
    float ndc_y = ((a_pos.y - area_h.x) / (area_h.y - area_h.x)) * 2.0 - 1.0;
    gl_Position = vec4(ndc_x, ndc_y, 0.0, 1.0);
}
